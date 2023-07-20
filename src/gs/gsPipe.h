/*
 *  gsPipe module:              
 *  Based on GFX-Pipe by Vzzrzzn (and modifications by Sjeep)
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
 

#ifndef _GSPIPE_H_
#define _GSPIPE_H_

#include "gsDefs.h"


	void gsPipeInit(unsigned int size); // buffer size must be less than 1MB
	
	// ReInit this pipe (re-setup alpha/zbuffer states etc) after using a different gsPipe
	void pipeReInit(void);
	
	void pipeInit(unsigned long *dmatadr);

	unsigned int pipeGetPipeSize(void);

	/***********************
	* Pipe Flush Functions *
	***********************/

	void pipeFlushCheck(void);
	void pipeFlush(void);
	void pipeFlushInt(void);

	/***********************************
	* Texture Upload & Setup Functions *
	***********************************/


	// send a byte-packed texture from RDRAM to VRAM
	// TBP = VRAM_address
	// TBW = buffer_width_in_pixels  -- dependent on pxlfmt
	// xofs, yofs in units of pixels
	// pxlfmt = 0x00 (32-bit), 0x02 (16-bit), 0x13 (8-bit), 0x14 (4-bit)
	// wpxls, hpxls = width, height in units of pixels

	void pipeTextureUpload(unsigned int TBP, int TBW, int xofs, int yofs, int pxlfmt, const unsigned char* tex, int wpxls, int hpxls);

	// send a byte-packed texture from VRAM to RDRAM
	// TBP = VRAM_address
	// TBW = buffer_width_in_pixels  -- dependent on pxlfmt
	// xofs, yofs in units of pixels
	// pxlfmt = 0x00 (32-bit), 0x02 (16-bit), 0x13 (8-bit), 0x14 (4-bit)
	// wpxls, hpxls = width, height in units of pixels

	void pipeTextureDownload(unsigned int TBP, int TBW, int xofs, int yofs, int pxlfmt, unsigned char* tex, int wpxls, int hpxls);

	void pipeTextureSet(unsigned int tbp, int tbw, int texwidth, int texheight, u32 tpsm, u32 cbp, u32 csm, u32 cbw, u32 cpsm);
	void pipeTextureFlush(void);
	void pipeSetFilterMethod(int FilterMethod);
	

	/***********************************
	* ZBuffer Enable/Disable Functions *
	***********************************/

	void pipeSetZTestEnable(int enable);
	void pipeSetAlphaEnable(int enable);

	void pipeSetScissorRect(long x1, long y1, long x2, long y2);
	void pipeSetOrigin(int x, int y);

	void pipeSetDither(unsigned long enable);
	void pipeSetColClamp(unsigned long enable);
	void pipeSetPrModeCont(unsigned long enable);


	/******************************************
	* Here are all the Point & line functions *
	******************************************/

	void pipePoint(
		int x, int y,
		unsigned z, unsigned colour);

	void pipeLine(
		int x1, int y1,
		int x2, int y2,
		unsigned z, unsigned colour);

	/**************************************
	* Here are all the triangle functions *
	**************************************/

	void pipeTriangleLine(
		int x1, int y1, unsigned z1, unsigned c1,
		int x2, int y2, unsigned z2, unsigned c2,
		int x3, int y3, unsigned z3, unsigned c3);

	void pipeTriangleFlat(
		int x1, int y1, unsigned z1,
		int x2, int y2, unsigned z2,
		int x3, int y3, unsigned z3,
		unsigned colour);

	void pipeTriangleGouraud(
		int x1, int y1, unsigned z1, unsigned c1,
		int x2, int y2, unsigned z2, unsigned c2,
		int x3, int y3, unsigned z3, unsigned c3);


	void pipeTriangleTexture(
		int x1, int y1, unsigned z1, unsigned u1, unsigned v1,
		int x2, int y2, unsigned z2, unsigned u2, unsigned v2,
		int x3, int y3, unsigned z3, unsigned u3, unsigned v3,
		unsigned colour);
	
	void pipeTriangleTextureGouraud(
		int x1, int y1, unsigned z1, unsigned u1, unsigned v1, unsigned c1,
		int x2, int y2, unsigned z2, unsigned u2, unsigned v2, unsigned c2,
		int x3, int y3, unsigned z3, unsigned u3, unsigned v3, unsigned c3);

	/***************************************
	* Here are all the rectangle functions *
	***************************************/

	void pipeRectFlat(
		int x1, int y1,
		int x2, int y2,
		unsigned z, unsigned colour);

	void pipeRectLine(
		int x1, int y1,
		int x2, int y2,
		unsigned z, unsigned colour);

	void pipeRectTexture(
		int x1, int y1, u32 u1, u32 v1,
		int x2, int y2, u32 u2, u32 v2,
		u32 z, u32 colour);


	void pipeRectGouraud(
		int x1, int y1, unsigned c1,
		int x2, int y2, unsigned c2,
		unsigned z);


	/**************************************
	* Here are all the TriStrip functions *
	**************************************/

	void pipeTriStripGouraud(
		int x1, int y1, unsigned z1, unsigned c1,
		int x2, int y2, unsigned z2, unsigned c2,
		int x3, int y3, unsigned z3, unsigned c3,
		int x4, int y4, unsigned z4, unsigned c4);

	void pipeTriStripGouraudTexture(
		int x1, int y1, unsigned z1, unsigned u1, unsigned v1, unsigned c1,
		int x2, int y2, unsigned z2, unsigned u2, unsigned v2, unsigned c2,
		int x3, int y3, unsigned z3, unsigned u3, unsigned v3, unsigned c3,
		int x4, int y4, unsigned z4, unsigned u4, unsigned v4, unsigned c4);



	// These 2 member functions should only be accessed by the gsDriver class
	void pipeSetZBuffer(unsigned long base, int psm, unsigned long mask);
	void pipeSetDrawFrame(unsigned long base, unsigned long width , int psm, unsigned long mask);


#endif
