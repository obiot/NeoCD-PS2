/*
 *  misc.c
 *  Copyright (C) 2001-2003 Foster (Original Code)
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

#ifndef	MISC_C
#define MISC_C

#include <tamtypes.h>
#include <kernel.h> 
#include <stdio.h>
#include <fileio.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h> 
#include "../defines.h" 


void swab( const void* src1, const void* src2, int isize)
{
	char*	ptr1;
	char*	ptr2;
	char	tmp;
	int	ic1;
	
	ptr1 = (char*)src1;
	ptr2 = (char*)src2;
	for ( ic1=0 ; ic1<isize ; ic1+=2){
		tmp = ptr1[ic1+0];
		ptr2[ic1+0] = ptr1[ic1+1];
		ptr2[ic1+1] = tmp;
	}
}
#endif
