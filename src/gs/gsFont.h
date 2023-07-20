/*
 *  gsFont module:
 *  Screen Font print function using gsLib's gsPipe
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


#ifndef _GSFONT_H_
#define _GSFONT_H_

#include "gsDefs.h"
#include "gsPipe.h"



enum gsFontAlignDed 
{
	GSFONT_ALIGN_LEFT = 1,
	GSFONT_ALIGN_CENTRE,
	GSFONT_ALIGN_RIGHT,
};

static const int gsFontAlign[3] = 
{ 	
	GSFONT_ALIGN_LEFT,
	GSFONT_ALIGN_CENTRE,
	GSFONT_ALIGN_RIGHT,
};

typedef struct
{
  	int pixl;
  	int charl;
} MYlength;


unsigned int m_TBbase;

// all bitmap characters should be left-aligned in PixelData (if kerning at least)

typedef struct{
	
	// overall texture size is in pixels, since there is no reason
	// why overall texture size has to be in GS_TEX units (ie: a power of 2)
	char ID[4];	//"BFNT"

	unsigned int TexWidth;	// overall width of texture - in pixels
	unsigned int TexHeight;	// overall width of texture - in pixels

	unsigned int PSM;	// Colour Depth of font Texture (in GS_PSM)

	unsigned int NumXChars;	// number of characters horizontally in grid
	unsigned int NumYChars;	// number of characters vertically in grid

	unsigned int CharGridWidth;	// width of one char grid - in pixels 
	unsigned int CharGridHeight; // height of one char grid - in pixels

	// 256 byte entry for width of individual characters
	char CharWidth[256];

	// actual bitmap pixel data
	char PixelData;
	
} gsFontTex;


void fontUploadFont(gsFontTex* pFontTex, unsigned int TBbase, int TBwidth, int TBxpos, int TBypos );

void fontPrint(int x, int Xend, int y, int z, unsigned long colour, int alignment, const char* string);



#endif //_GSFONT_H_

