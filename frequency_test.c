#include <unistd.h>
#include <stdio.h>
#include "memory_api.h"

#define NUM_TESTS   8
#define TEST_DELAY  1 //10

#define LFSR_32BIT_ADDRESS      0x43C00000

#define ARM_PLL_CTRL_ADDR 0xF8000100
#define ARM_PLL_RST_VALUE 0x0001A008
#define PLL_FDIV          0x1A

#define ARM_PLL_CFG_ADDR  0xF8000110
#define LOCK_CNT          0xFA000
#define LOCK_CNT_MASK     0x3FF000
#define PLL_CP              0x200
#define PLL_CP_MASK         0xF00
#define PLL_RESET           0x20
#define PLL_RESET_MASK      0xF0


#define ARM_PLL_STATUS_ADDR   0xF800010C

#define ARM_CLK_CTRL_ADDR     0xF8000120

//1. Program the ARM_PLL CTRL[PLL_FDIV] value and the PLL configuration register, ARM_PLL_CFG[LOCK_CNT, PLL_CP, PLL_RESET], to their required values
//2. Force the PLL into bypass mode by writing a 1 to ARM_PLL_CTRL [PLL_BYPASS_FORCE<4>] and setting the ARM_PLL_CTRL [PLL_BYPASS_QUAL<3>] bit to a 0
//3. Assert and de-assert the PLL reset by writing a 1 and then a 0 to ARM_PLL_CTRL [PLL_RESET<0>]
//4. Verify that the PLL is locked by reading PLL_STATUS [ARM_PLL_LOCK<3>]
//5. Disable the PLL bypass mode by writing a 0 to ARM_PLL_CTRL [PLL_BYPASS_FORCE<4>]
//6. Set the clock divider

int change_frequency(unsigned int pll_divider, unsigned int clock_divider);

static unsigned int PllDividersTestArray[NUM_TESTS]   = { 40, 44, 20, 48, 2, 34, 18, 48};
static unsigned int ClockDividersTestArray[NUM_TESTS] = {  2,  2, 12,  3, 2, 20, 36,  2};
#define BASE_CLK 33

int change_frequency(unsigned int pll_divider, unsigned int clock_divider)
{
  int result = -1;
  unsigned int data_cfg     = (unsigned int) 0;
  unsigned int data_ctrl    = (unsigned int) 0;
  unsigned int data_status  = (unsigned int) 0;
  unsigned int data_clk_div = (unsigned int) 0;

  /**
   * The PLL configuration register, ARM_PLL_CFG[LOCK_CNT, PLL_CP, PLL_RESET], to their required values
   */
  dm(ARM_PLL_CFG_ADDR, &data_cfg);
  printf("1. Current PLL configuration register = %x\n", data_cfg);
  data_cfg = data_cfg | (LOCK_CNT_MASK & LOCK_CNT) | (PLL_CP_MASK & PLL_CP) | (PLL_RESET_MASK & PLL_RESET);
  pm(ARM_PLL_CFG_ADDR, &data_cfg);
  printf("1. Configured PLL register = %x\n", data_cfg);

  /**
   * 2. Program the ARM_PLL CTRL[PLL_FDIV] value and
   * Force the PLL into bypass mode by writing a 1 to
   * ARM_PLL_CTRL [PLL_BYPASS_FORCE<4>] and setting the
   * ARM_PLL_CTRL [PLL_BYPASS_QUAL<3>] bit to a 0
   */
  // Configuring divider
  dm(ARM_PLL_CTRL_ADDR, &data_ctrl);
  printf("2. Current PLL Control Register = %x\n", data_ctrl);
  data_ctrl = data_ctrl | 0x10;
  data_ctrl = data_ctrl & ~0x8;
  pm(ARM_PLL_CTRL_ADDR, &data_ctrl);
  data_ctrl = (data_ctrl & ~0x0007F000) | (pll_divider << 12);
  pm(ARM_PLL_CTRL_ADDR, &data_ctrl);
  printf("2. Configured PLL Control Register = %x\n", data_ctrl);

  /**
   * 3. Assert and de-assert the PLL reset by writing a 1 and then a 0 to
   * ARM_PLL_CTRL [PLL_RESET<0>]
   */
  // Writing 1
  dm(ARM_PLL_CTRL_ADDR, &data_ctrl);
  printf("3. Current PLL Control Register = %x\n", data_ctrl);
  data_ctrl = data_ctrl | 0x1;
  pm(ARM_PLL_CTRL_ADDR, &data_ctrl);

  // Writing 0
  dm(ARM_PLL_CTRL_ADDR, &data_ctrl);
  data_ctrl = data_ctrl & ~0x1;
  pm(ARM_PLL_CTRL_ADDR, &data_ctrl);
  printf("3. Configured PLL Control Register = %x\n", data_ctrl);

  /**
   * 4. Verify that the PLL is locked by reading PLL_STATUS [ARM_PLL_LOCK<3>]
   */
  dm(ARM_PLL_STATUS_ADDR, &data_status);
  printf("4. Current PLL Status Register = %x\n", data_status);
  while((data_status & 0x01) != 0x01) // TODO- verify if is bit 0 or bit 3
  {
    dm(ARM_PLL_STATUS_ADDR, &data_status);
    printf("4. Current PLL Status Register = %x\n", data_status);
  }
  printf("4. ARM PLL Locked : data_status = %x\n", data_status);

  /**
   * 5. Disable the PLL bypass mode by writing a 0 to ARM_PLL_CTRL [PLL_BYPASS_FORCE<4>]
   */
  dm(ARM_PLL_CTRL_ADDR, &data_ctrl);
  printf("5. Current PLL Control Register = %x\n", data_ctrl);
  data_ctrl = data_ctrl & ~0x10;
  pm(ARM_PLL_CTRL_ADDR, &data_ctrl);
  printf("5. Configured PLL Control Register = %x\n", data_ctrl);

  /**
   * Set the clock divider
   */
  dm(ARM_CLK_CTRL_ADDR, &data_clk_div);
  printf("6. Current CLK Control Register = %x\n", data_clk_div);
  data_clk_div = (data_clk_div & ~0x00003f00) | (clock_divider << 8);
  pm(ARM_CLK_CTRL_ADDR, &data_clk_div);
  printf("6. Current CLK Control Register = %x\n", data_clk_div);

  return result;
}

int main()
{
  unsigned int i = 0;
  int delay;

  for(i=0; i < NUM_TESTS; ++i)
  {
    dm (LFSR_32BIT_ADDRESS, &delay);
    delay = ((delay & 0xefffffff) % 2) + 1;
    printf("TEST NUMBER %d with PLL Divider %d and Clock Divider %d: %d MHZ\n", i+1, PllDividersTestArray[i], ClockDividersTestArray[i], (BASE_CLK*PllDividersTestArray[i])/ClockDividersTestArray[i]);
    // Call change frequency
    change_frequency(PllDividersTestArray[i],ClockDividersTestArray[i]);
    printf("Waiting for %d seconds\n", delay);
    sleep(delay);
  }

  return 0;
}
