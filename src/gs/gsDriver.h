/*
 *  gsDriver module: 
 *  Provides functions for initialisation and configuration of the PS2's GS,    
 *  aswell as support for GS management and multiple frame-buffers.  
 *
 *  Copyright (C) 2005 Olivier "Evilo" Biot
 *  Copyright (C) 2003 A.Lee "Hyriu"
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
 *
 */

#ifndef _GSDRIVER_H_
#define _GSDRIVER_H_

#include "gsDefs.h"
#include "gsPipe.h"


static const int bufTexSize[10] = 
{ 	GS_TEX_SIZE_2,
	GS_TEX_SIZE_4,
	GS_TEX_SIZE_8,
	GS_TEX_SIZE_16,
	GS_TEX_SIZE_32,
	GS_TEX_SIZE_64,
	GS_TEX_SIZE_128,
	GS_TEX_SIZE_256,
	GS_TEX_SIZE_512,
	GS_TEX_SIZE_1024
};

// Default init
void gsInit(void);

// Setup Display for specified size, mode, and number of buffers
void gsSetDisplayMode(unsigned int width, unsigned int height,
		      unsigned int xpos, unsigned int ypos,
		      unsigned int psm, unsigned int num_bufs,
		      unsigned int TVmode, unsigned int TVinterlace,
		      unsigned int zbuffer, unsigned int zpsm);


void gsClearScreen(void);

void gsSetDisplayPosition(unsigned int xpos, unsigned int ypos);

// Get the location of the frame buffer or texture buffer
unsigned int gsGetFrameBufferBase(unsigned int fb_num);
unsigned int gsGetTextureBufferBase(void);

// Get the number of the current display or draw buffers
unsigned int gsGetCurrentDisplayBuffer(void);
unsigned int gsGetCurrentDrawBuffer(void);

static inline unsigned int gsGetBytesPerPixel(unsigned int psm)
{
	switch (psm)
	{
	case GS_PSMCT32:
		return 4;
	case GS_PSMCT24:
	case GS_PSGPU24:
		return 3;
	case GS_PSMCT16:
	case GS_PSMCT16S:
		return 2;
		default:
		return 1;
	}
}

static inline int gsGetTexSizeFromInt(int texsize)
{
	// Get the size in 2^X of texsize
	int pow2 = 0x0400; // exact power of 2
	int i;
		// special case for 0
	if (texsize == 0)
		return bufTexSize[0];
		for (i=10; i>=0; i--)
	{
		// If texsize = exact 2^X
		if (texsize == pow2)
		{
			return bufTexSize[i];
		}
			// check if texsize is bigger than the next lower power of 2
		// (also handles the case of a texsize > 1024)
		if (texsize > (pow2>>1))
		{
			return bufTexSize[i];
		}
			pow2 = pow2 >> 1;
	}
		return bufTexSize[0];
}

static inline void gsWaitForVSync(void)
{
	GS_CSR &= 8; // generate
	while(!(GS_CSR & 8)); // wait until its generated
}

//VSync Interrupt Handler Routines 
unsigned int gsAddVSyncCallback(void (*func_ptr)());
void gsRemoveVSyncCallback(unsigned int RemoveID);
void gsEnableVSyncCallbacks(void);
void gsDisableVSyncCallbacks(void);


// Only to be used for traditional double-buffer display
// (Use other funcs for multi-buffer displays)
void gsSwapBuffers(void);


/************************
* Multiple-Buffer funcs *
************************/


// Check if there is free buffer for Drawing to
int gsIsDrawBufferAvailable();

// Check if there is a completed buffer available for displaying
int gsIsDisplayBufferAvailable();

// Set the draw environment to the next free buffer
// If no free buffer is available to draw then do nothing
// (continuing drawing will draw over existing frame, so
// to avoid this poll isDrawBufferAvailable until TRUE)
void gsSetNextDrawBuffer();

// Call this function when you have finished drawing to a frame
// to indicate that it is available for display
void gsDrawBufferComplete();	
	
// Call this on vsync to display the next frame (if complete)
// and free-up another buffer for drawing.
// no need to call DisplayBufferFree() or setNextDisplayBuffer()
void gsDisplayNextFrame();
	


#endif /* _GSDRIVER_H_ */
