/*
 *  draw_fix.h - Fixed Text Layer Drawing Routines
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

#include <stdio.h>
#include <string.h>

#include "video.h"
#include "neocd.h"


/* Draw Single FIX character */

INLINE void draw_fix(u16 code, u16 colour, u16 sx, u16 sy)
{
	u8 y;
	u32 mydword;
	u32 * fix=(u32*)&(neogeo_fix_memory[code<<5]);
	u16 * dest = video_line_ptr[sy] + sx;
	u16 * paldata=&video_paletteram_pc[colour];
	u16 col;
	
	for( y=8; y--; )
	{
		mydword  = *fix++;
		
		col = (mydword>> 0)&0x0f; if (col) dest[0] = paldata[col];
		col = (mydword>> 4)&0x0f; if (col) dest[1] = paldata[col];
		col = (mydword>> 8)&0x0f; if (col) dest[2] = paldata[col];
		col = (mydword>>12)&0x0f; if (col) dest[3] = paldata[col];
		col = (mydword>>16)&0x0f; if (col) dest[4] = paldata[col];
		col = (mydword>>20)&0x0f; if (col) dest[5] = paldata[col];
		col = (mydword>>24)&0x0f; if (col) dest[6] = paldata[col];
		col = (mydword>>28)&0x0f; if (col) dest[7] = paldata[col];
		dest += BUF_WIDTH;
	}
}
 
/* Draw entire Character Foreground */
INLINE void video_draw_fix(void)
{

	register uint16 x, y;
	register uint16 code, colour;
	uint16 * fixarea=(uint16 *)&video_vidram[0xe004];

	for (y=0; y < 28; y++)
	{
		for( x=40; x--; )
		{
			code = fixarea[x << 5];
			colour = (code&0xf000)>>8;
			code  &= 0xfff;
			if (video_fix_usage[code])
  				draw_fix(code,colour,x<<3,y<<3);
		}
		fixarea++;
	}
}


