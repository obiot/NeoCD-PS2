/*
 *  video.h - Video Hardware Emulation
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
 
#ifndef	VIDEO_H
#define VIDEO_H

#include "neocd.h"
#include "defines.h"

/*-- Defines ---------------------------------------------------------------*/
#define PAL_MODE 50
#define NTSC_MODE 60

#define BUF_WIDTH	336 // 336 // size should be 320, but it causes gfx glitches ...
#define BUF_HEIDTH	224

#define TextOutC2(x_start, x_end, y, string,  z) textCpixel((x_start)>>4,(x_end)>>4,((y)>>4)+4,GS_SET_RGBA(255,255,255,128),0,0,(z),(string))


/*-- Global Variables ------------------------------------------------------*/
extern char		*video_vidram ;// __attribute__((aligned(64)));
extern unsigned short	*video_paletteram_ng __attribute__((aligned(64)));
extern unsigned short	video_palette_bank0_ng[4096] __attribute__((aligned(64)));
extern unsigned short	video_palette_bank1_ng[4096] __attribute__((aligned(64)));
extern unsigned short	*video_paletteram_pc __attribute__((aligned(64)));
extern unsigned short	video_palette_bank0_pc[4096] __attribute__((aligned(64)));
extern unsigned short	video_palette_bank1_pc[4096] __attribute__((aligned(64)));
extern short		video_modulo;
extern unsigned short	video_pointer;
extern unsigned short	*video_paletteram __attribute__((aligned(64)));
extern unsigned short	*video_paletteram_pc __attribute__((aligned(64)));
extern unsigned short	video_palette_bank0[4096] __attribute__((aligned(64)));
extern unsigned short	video_palette_bank1[4096] __attribute__((aligned(64)));
extern unsigned short	*video_line_ptr[224] __attribute__((aligned(64)));
//extern unsigned char	video_fix_usage[4096] __attribute__((aligned(64)));
extern unsigned char	*video_fix_usage;
//extern unsigned char	rom_fix_usage[4096] __attribute__((aligned(64)));
extern unsigned char	*rom_fix_usage;
extern unsigned char	video_spr_usage[32768] __attribute__((aligned(64)));
extern unsigned char	rom_spr_usage[32768] __attribute__((aligned(64)));
extern unsigned int	video_hide_fps;
extern unsigned short	video_color_lut[32768] __attribute__((aligned(64)));

// pointer to the framebuffer
extern uint32 		*video_frame_buffer_ptr;

extern double		gamma_correction;
extern int		frameskip;

extern unsigned int	neogeo_frame_counter;
extern unsigned int	neogeo_frame_counter_speed;


extern int 		whichdrawbuf;

extern unsigned int 	dpw;
extern unsigned int 	dph;


/*-- video.c functions ----------------------------------------------------*/
int  video_init(void);
void setVideoModeConfig(int mode);
int  video_set_mode();
void video_precalc_lut(void);
void video_draw_screen1(void);
inline void video_draw_spr(unsigned int code, unsigned int color, int flipx,
			int flipy, int sx, int sy, int zx, int zy);
void video_setup(void);
void incframeskip(void);
void clean_buffers(void);
void blitter(void);
void system_graphics_blit(void);

/*-- draw_fix.c functions -------------------------------------------------*/
void video_draw_fix(void);
//void clean_buffers(void);

void inline ps2_vsync();

void inline ps2_switch_buffers();


void inline displayGPFrameBuffer(uint32 *buffer);
void inline displayGPImageBuffer(uint32 *buffer);

#endif /* VIDEO_H */

