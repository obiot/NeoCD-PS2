/*
 *  video_util.c - video conversion routines & misc
 *  Copyright (C) 2004-2005 Olivier "Evilo" Biot (PS2 Port)
 *  Copyright (C) 2001 Peponas Mathieu (GNGEO)
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
 
#include "neocd.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void convert_all_char(uint8 *Ptr, int Taille, 
		      uint8 *usage_ptr)
{
    int		i,j;
    unsigned char	usage;
    
    uint8 *Src;
    uint8 *sav_src;

    Src=(uint8*)memalign(64,Taille);
    if (!Src) {
	printf("Not enough memory!!\n");
	return;
    }
    sav_src=Src;
    memcpy(Src,Ptr,Taille);
    
  #define CONVERT_TILE *Ptr++ = *(Src+16);\
	               usage |= *(Src+16);\
                       *Ptr++ = *(Src+24);\
		       usage |= *(Src+24);\
		       *Ptr++ = *(Src);\
		       usage |= *(Src);\
		       *Ptr++ = *(Src+8);\
		       usage |= *(Src+8);\
		       Src++;
  
    for(i=Taille;i>0;i-=32) {
        usage = 0;
        for (j=0;j<8;j++) {
            CONVERT_TILE
                }
        Src+=24;
        *usage_ptr++ = usage;
    }
    free(sav_src);
  #undef CONVERT_TILE
}


/* For MGD-2 dumps */

/*
static int mgd2_tile_pos=0;
void convert_mgd2_tiles(unsigned char *buf,int len)
{
    int i;
    unsigned char t;


    if (len==memory.gfx_size && mgd2_tile_pos==memory.gfx_size) 
    {
	mgd2_tile_pos=0;
    }

    if (len == 2) 
    {
	return;
    }

    if (len == 6)
    {
        unsigned char swp[6];

        memcpy(swp,buf,6);
        buf[0] = swp[0];
        buf[1] = swp[3];
        buf[2] = swp[1];
        buf[3] = swp[4];
        buf[4] = swp[2];
        buf[5] = swp[5];

        return;
    }

    if (len % 4) exit(1);	// must not happen

    len /= 2;

    for (i = 0;i < len/2;i++)
    {
        t = buf[len/2 + i];
        buf[len/2 + i] = buf[len + i];
        buf[len + i] = t;
    }
    if (len==2) {
	mgd2_tile_pos+=2;
	if ((mgd2_tile_pos&0x3f)==0)  update_progress_bar(mgd2_tile_pos,memory.gfx_size);
    }
    convert_mgd2_tiles(buf,len);
    convert_mgd2_tiles(buf + len,len);
}
*/

