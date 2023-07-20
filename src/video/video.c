/*
 *  video.c - Video Hardware Emulation
 *  Copyright (C) 2003 Foster (Original Code)
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

//-- Include Files -----------------------------------------------------------

#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <string.h>
//#include <libpad.h>

#include "video.h"
#include "../input/input.h"
#include "../misc/misc.h"
#include "../misc/timer.h"

#include "../gs/clut.h"




//-- Global Variables --------------------------------------------------------
char *video_vidram;
//unsigned short *video_vidram;
unsigned short	*video_paletteram_ng __attribute__((aligned(64)));
unsigned short	video_palette_bank0_ng[4096] __attribute__((aligned(64)));
unsigned short	video_palette_bank1_ng[4096] __attribute__((aligned(64)));
unsigned short	*video_paletteram_pc __attribute__((aligned(64)));
unsigned short	video_palette_bank0_pc[4096] __attribute__((aligned(64)));
unsigned short	video_palette_bank1_pc[4096] __attribute__((aligned(64)));
unsigned short	video_color_lut[32768] __attribute__((aligned(64)));

short		video_modulo;
unsigned short	video_pointer;

unsigned short 	*video_line_ptr[224] __attribute__((aligned(64)));

unsigned char	*video_fix_usage;//[4096] __attribute__((aligned(64))) ;

unsigned char	*rom_fix_usage;//[4096] __attribute__((aligned(64)));

unsigned char	video_spr_usage[32768] __attribute__((aligned(64))); 
unsigned char   rom_spr_usage[32768] __attribute__((aligned(64))); 

unsigned int dpw = 320;
unsigned int dph = 224;


static const u64 TEXTURE_RGBA 	    = GS_SET_RGBA(0x80, 0x80, 0x80, 0x80);
static const u64 WHITE_RGBA 	    = GS_SET_RGBA(0xFF, 0xFF, 0xFF, 0xFF);
static const u64 CLEAR_TEXTURE_RGBA = GS_SET_RGBA(0, 0, 0, 0x80);

static uint16 	video_buffer[ BUF_WIDTH * BUF_HEIDTH ] __attribute__((aligned(64))) __attribute__ ((section (".bss"))); // 320*224, rest is padding

static uint32 	video_frame_buffer[ BUF_WIDTH * BUF_HEIDTH ] __attribute__((aligned(64)))  __attribute__ ((section (".bss"))); // 320*224, rest is padding

// pointer to the frame buffer
uint32 		*video_frame_buffer_ptr;

char 		display_fps[12]  __attribute__((aligned(64))); 
extern int 	fps; 

extern volatile int frame_counter;

extern unsigned vsync_freq; 


//static uint16	*src_blit;  // blitter pointer
//static uint32	*dest_blit; // blitter pointer

unsigned char	*dda_y_skip; // scratchpad
unsigned char	*full_y_skip;//[16] __attribute__ ((section (".sdata"))) = {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};



unsigned int	neogeo_frame_counter = 0;
unsigned int	neogeo_frame_counter_speed = 4;


int		frameskip=0,xoffs,yoffs,offs;



//----------------------------------------------------------------------------
int	video_init(void)
{

	int		y,i;

	video_precalc_lut();

	video_vidram = malloc(131072);

	if (video_vidram==NULL) {
		printf("VIDEO: Could not allocate vidram (128k)\n");
		return	0;
	}

	memset((void*)video_palette_bank0_ng, 0, sizeof(video_palette_bank0_ng));
	memset((void*)video_palette_bank1_ng, 0, sizeof(video_palette_bank1_ng));
	memset((void*)video_palette_bank0_pc, 0, sizeof(video_palette_bank0_pc));
	memset((void*)video_palette_bank1_pc, 0, sizeof(video_palette_bank1_pc));

	video_paletteram_ng = video_palette_bank0_ng;
	video_paletteram_pc = video_palette_bank0_pc;
	video_modulo = 1;
	video_pointer = 0;

        for(i=0;i<32768;i++)
            video_spr_usage[i]=1;

	for(y=0;y<BUF_HEIDTH;y++) {
		video_line_ptr[y] = video_buffer + y * BUF_WIDTH; // 512
	}
	
	// zero-fill both buffers
	memset((void*)video_buffer, 0, sizeof(video_buffer)); 
	memset((void*)video_frame_buffer, 0, sizeof(video_frame_buffer)); 
	
	// assign public pointer
	video_frame_buffer_ptr = video_frame_buffer;
	

	// put in scratchpad
	dda_y_skip = (unsigned char *) sprAlloc(32); 
	
	full_y_skip = (unsigned char *) sprAlloc(32); 
	full_y_skip[0]=	0;
	for(i=1;i<17;i++)
            full_y_skip[i]= 1 ;
		
	video_fix_usage = (unsigned char*) sprAlloc(4096); 
	for(i=0;i<4096;i++)
            video_fix_usage[i]=0;
        
        rom_fix_usage = (unsigned char*) sprAlloc(4096); 
	for(i=0;i<4096;i++)
            rom_fix_usage[i]=0;
	
	return video_set_mode();
}

uint8 gs_is_pal(void)
{
	if(*((char *)0x1FC80000 - 0xAE) == 'E') return(1);
	return(0);
} 

void setVideoModeConfig(int mode)
{
	if(mode == PAL_MODE)
	{
		machine_def.dispx = neocdSettings.dispXPAL;
		machine_def.dispy = neocdSettings.dispYPAL;
		machine_def.vdph = 256;
		machine_def.vidsys = PAL_MODE;
		neocdSettings.region = REGION_EUROPE;
		machine_def.fps_rate = FPS_PAL; //50;
		machine_def.snd_sample = 240; //12000khz /50
		machine_def.m68k_cycles = 12000000 / 50; // 240.000
		machine_def.z80_cycles = 4000000 / 50;
		machine_def.z80_cycles_slice = machine_def.z80_cycles / 256;
		vsync_freq = GETTIME_FREQ_PAL; 
		
	} 
	else // NTSC
	{
		machine_def.dispx = neocdSettings.dispXNTSC;
		machine_def.dispy = neocdSettings.dispYNTSC;
		machine_def.vdph = 224;
		machine_def.vidsys = NTSC_MODE;
        	neocdSettings.region = REGION_USA;
        	machine_def.fps_rate = FPS_NTSC; // 60;
        	machine_def.snd_sample = 200; //12000khz /60
        	machine_def.m68k_cycles = 12000000 / 60; // 200.000
        	machine_def.z80_cycles = 4000000 / 60;
		machine_def.z80_cycles_slice = machine_def.z80_cycles / 256;
		vsync_freq = GETTIME_FREQ_NTSC; 
	}	
} 
//----------------------------------------------------------------------------
int video_set_mode()
{

	gsInit();

	if(gs_is_pal()) // PAL
	{
		setVideoModeConfig(PAL_MODE);
	} 
	else // NTSC
	{
		setVideoModeConfig(NTSC_MODE);
	}
	
	// set video mode
	
	gsSetDisplayMode(320, machine_def.vdph, 
			 machine_def.dispx, machine_def.dispy, 
			 GS_PSMCT32,5, GS_TV_AUTO, GS_TV_NONINTERLACE,
			 GS_ENABLE, GS_PSMZ32);	
	
	pipeSetAlphaEnable(GS_ENABLE);

	
	// beurk..
	machine_def.y1_offset = ((machine_def.vdph-dph)>>1) ;
	machine_def.y2_offset = (machine_def.vdph-((machine_def.vdph-dph)>>1));
	


	// vsync callback
    	gsAddVSyncCallback(&system_graphics_blit); 
    	
    	gsClearScreen();
   	
	// display loading screen...
	display_loadingscreen();
   	
   	return machine_def.vidsys;
}


void incframeskip(void)
{
    frameskip++;
    frameskip=frameskip%12;
}
//----------------------------------------------------------------------------

void video_precalc_lut(void)
{
	int	ndx, rr, rg, rb;
	
	double	gamma_correction = 1.0;
	
	for(rr=0;rr<32;rr++) {
		for(rg=0;rg<32;rg++) {
			for(rb=0;rb<32;rb++) {
				ndx = ((rr&1)<<14)|((rg&1)<<13)|((rb&1)<<12)|((rr&30)<<7)
					|((rg&30)<<3)|((rb&30)>>1);
				video_color_lut[ndx] =
				     ((int)( 31 * pow( (double)rb / 31, 1 / gamma_correction ) )<<0)
					|((int)( 63 * pow( (double)rg / 31, 1 / gamma_correction ) )<<5)
					|((int)( 31 * pow( (double)rr / 31, 1 / gamma_correction ) )<<11);
			}
		}
	}
	
}



// called by the vsync interrupt
void system_graphics_blit(void)
{
   
   if (gsIsDisplayBufferAvailable())
   {
   	gsDisplayNextFrame();
   	frame_counter++;
   }   
   asm __volatile__ ("ei");
}


/*------------------------------------------------------------*/
/* blit image to screen */
void blitter(void)
{
	// make an awfull RGB565 to 32bit color conversion
	// faster way to do it ???	
	// this should be made using the hardware
	
 	// bypass the cache mechanism
	//uint16 *src_blit  = (uint16 *) ((u32)video_buffer | 0x20000000);
	uint16 *src_blit  = (uint16 *) (video_buffer);
	
	//uint32 *dest_blit = (uint32 *) ((u32)video_frame_buffer | 0x20000000);
	uint32 *dest_blit = (uint32 *) (video_frame_buffer);
	
	//uint32 *clut_bit = (uint32 *) (&CLUT);
		
	for( offs= ( BUF_WIDTH * BUF_HEIDTH ); offs--; )
	{	
	   *dest_blit++ = CLUT[(uint16)*src_blit++];
	}   
	   //*dest_blit++ = (uint32)*clut_bit+(*src_blit++);

	//}

	displayGPFrameBuffer(video_frame_buffer);
	
	// Indicate that we have finished drawing
	gsDrawBufferComplete();

	// if no new buffer is available for drawing, then wait until one is
	while(!(gsIsDrawBufferAvailable()));

	// once buffer is available then set the next display buffer and continue drawing
	gsSetNextDrawBuffer(); 
	
}
 

/*-- UI functions -------------------------------------------------*/
// display a 512x256x32 image buffer using GfxPipe
void inline displayGPFrameBuffer(uint32 *buffer)
{	
	
	// clear the screen
	pipeSetZTestEnable(GS_DISABLE);
	pipeRectFlat(0,0,320,machine_def.vdph,0,CLEAR_TEXTURE_RGBA);
	pipeSetZTestEnable(GS_ENABLE);
	
	// Upload into the beginning of texture mem (with texture-buffer width set to 512)
	pipeTextureUpload(
		gsGetTextureBufferBase(), 512,
		0, 0, (int)GS_PSMCT32, (const unsigned char*)&buffer[0],
		BUF_WIDTH, BUF_HEIDTH);

	// Set-up the Texture (can do this outside the draw loop in this example, since we are only using one texture)
	pipeTextureSet(
		gsGetTextureBufferBase(), 512,
		GS_TEX_SIZE_512, GS_TEX_SIZE_256,GS_PSMCT32, 
		0, 0, 0, 0); 
		
	// Draw a moving, solid, textured rectangle, with our texture on it (at z=1)
	pipeRectTexture(
		0, machine_def.y1_offset, 	// x1,y1
		0, 0,				// u1,v1
		320, machine_def.y2_offset, 	// x2 (320) ,y2
		320, 224,			// u2 (320), v2(224)
		10, 				// z
		//GS_SET_RGBA(255,255,255,200)
		TEXTURE_RGBA//GS_SET_RGBA(0x80, 0x80, 0x80, 0x80)
		); 


#ifdef BETA
	// add it into the menu option ???
	if (neocdSettings.showFPS)
	{
		sprintf(display_fps,"FPS %d/%d",fps,machine_def.fps_rate);
		textpixel(10,5,WHITE_RGBA  ,0,0,12,display_fps);
	}
#endif		 

		
	// Flush the pipe, to draw the prims
	pipeFlush(); 

}

//------------------------------------------------------------------
// display a 256x256x32 fixed image buffer using GfxPipe
void inline displayGPImageBuffer(uint32 *buffer)
{
	// Upload into the beginning of texture mem (with texture-buffer width set to 256)
	pipeTextureUpload(
		gsGetTextureBufferBase(), 256,
		0, 0, (int)GS_PSMCT32, (const unsigned char*)&buffer[0],
		256, 256);
	
	// Set-up the Texture (can do this outside the draw loop in this example, since we are only using one texture)
	pipeTextureSet(
		gsGetTextureBufferBase(), 256,
		GS_TEX_SIZE_256, GS_TEX_SIZE_256,GS_PSMCT32, 
		0, 0, 0, 0); 

	// Draw a moving, solid, textured rectangle, with our texture on it (at z=1)
	pipeRectTexture(
		32, 0, 			// x1,y1
		0, 0, 			// u1,v1
		(256+32), 256,		// x2,y2
		256, 256, 		// u2,v2
		10, 				// z
		//GS_SET_RGBA(255,255,255,200)
		GS_SET_RGBA(0x80, 0x80, 0x80, 0x80)
		); 
		
		
	// Flush the pipe, to draw the prims
	pipeFlush(); 

}



