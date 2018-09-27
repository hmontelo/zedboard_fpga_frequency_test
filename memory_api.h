/*
 * memory_api.h
 * author: Team 3
 * Created: Feb 14, 2018
 *
 *  dm.c, pm.c
 *
 *  author: Mark McDermott
 *  Created: Feb 12, 2012
 *
 *  Simple utility to allow the use of the /dev/mem device to display memory
 *  and write memory addresses on the i.MX21.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/*
** dm function for reading memory at a single location.
** inputs: addr is the address from which to read
** returns data in dat
** a status code of 0 is success
*/
int dm(unsigned long addr, unsigned int* dat);

/*
** dm function for writing to memory at a single location.
** inputs: addr is the address at which to write, dat is the location of data to write
** a status code of 0 is success
*/
int pm(unsigned long addr, unsigned int* dat);
