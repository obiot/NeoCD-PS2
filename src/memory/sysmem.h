/*
 *  sysmem.h - System Memory function
 *  Copyright (C) 2004-2005 Olivier "Evilo" Biot (PS2 Port)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
 
#ifndef __SYS_MEM_H_
#define __SYS_MEM_H_

#include <tamtypes.h>

/* Memory Map
 * The range 0x00000000 - 0x000fffff is reserved by the EE kernel and
 * cannot be accessed by applications
 *
 * 0x00000000-0x000fffff                 EE Kernel (Reserved)
 * 0x00100000-0x01ffffff                 Cached Area
 * 0x10000000-0x13ffffff                 EE and GS Registers (Uncached)
 * 0x20100000-0x21ffffff                 Uncached area
 * 0x30100000-0x31ffffff                 Uncached accelerated area (UCAB)
 * 0x70000000-0x70003fff                 Scratch Pad RAM
 *
 * Uncached Memory means that reads do not go through the cache and
 * writes go through the write back buffer. (Continuous stores of 128 bytes or less are done all at once)
 *
 * Uncached Accelerated Memory means that reads goes through the uncached
 * accelerated buffer (128 bytes read all at once) and writes go through the
 * write back buffer (Continuous stores of 128 bytes or less are done all at once)
 *
 * Uncached accelerated is a 1st choice for accessing contiguous memory without affecting the
 * cache and Uncached accelerated is faster except for random memory access
 */

#define SPR_START           0x70000000
#define SPR_END             0x70003FFF
#define SPR_SIZE            0x3FFF

// Allowed UCAB and UNCACHED buffer sizes must be checked
#define UCAB_START          0x30100000
#define UCAB_END            0x31FFFFFF
#define UCAB_ADDR_MASK      0x30000000

#define UNCACHED_START      0x20100000
#define UNCACHED_ADDR_MASK  0x20000000


void * sprAlloc( int Size );
void   sprFree( int Size );
u32    sprAvailable(void);

inline void *  getUCABPointer(void * ptr);
inline void *  getUncachedPointer(void * ptr);

#endif /* __SYS_MEM_H_ */
