/*
 *  sysmem.c - System Memory function
 *  Copyright (C) 2004-2005 Olivier "Evilo" Biot (Neocd PS2 Port)
 *  Copyright (C) 2005 - Froggies <evilo | shazz | 7not6 | tmator> 
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
#include "sysmem.h"
#include <string.h>

static void* gp_Spr      = (void*)SPR_START;

void* sprAlloc( int Size )
{
        void* p_spr = gp_Spr;

        if( gp_Spr+Size >= ( ((void*)SPR_START) + SPR_SIZE) )
        gp_Spr = (void*)SPR_START;

        p_spr = gp_Spr;
        gp_Spr += Size;

        return p_spr;
}

u32 sprAvailable(void)
{
    return ( ( (void*)SPR_START)+SPR_SIZE ) - gp_Spr;
}

void sprFree( int Size )
{
        gp_Spr -= Size;

        if( gp_Spr <= ( (void*)SPR_START)  )
        gp_Spr = (void*)SPR_START;
        
        memset(gp_Spr, 0x00, Size);

}

inline void * getUCABPointer(void * ptr)
{
     return  (void*)( (u32)ptr | UCAB_ADDR_MASK);

}

inline void * getUncachedPointer(void * ptr)
{
     return  (void*)( (u32)ptr | UNCACHED_ADDR_MASK);
}
