/*
 *  menu.c
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
 

#include <string.h>
#include <stdio.h>
#include <libpad.h>
#include <audsrv.h> 

#include "neocd.h"
#include "../save/mc.h"
#include "../video/video.h"
#include "../input/input.h"


// resources
#include "resources/splash.h"
#include "resources/loading.h"
#include "resources/credit.h"


#define MAX_MENU_ITEM 10

void IngameMenu()
{
	struct padButtonStatus pad1;
	int pad1_data = 0;
	int old_pad = 0;
	int new_pad;
	int ret;
	int selection = 0;
        static int ypos[MAX_MENU_ITEM] = {32<<4, 50<<4,68<<4,86<<4,104<<4,122<<4,140<<4,158<<4,176<<4,194<<4};
        int center_x,center_y;
        
        int settingsChanged = 0;


	if (neocdSettings.soundOn)
	   audsrv_set_volume(MIN_VOLUME);
	
	if(neocdSettings.CDDAOn)
	   cdda_pause();
	
	   
	center_x = 0;
	center_y = (machine_def.vdph-224)/2;

	
     	// main loop
	while(1) 
	{	
		
	
		//gsClearScreen();
		pipeSetZTestEnable(GS_DISABLE);
		pipeRectFlat(0,0,320,machine_def.vdph,0,GS_SET_RGBA(0,0,0,0x80));
		pipeSetZTestEnable(GS_ENABLE);
		
		
		// Set-up the Texture 
		pipeTextureSet(	gsGetTextureBufferBase(), 512,
				GS_TEX_SIZE_512, GS_TEX_SIZE_256,GS_PSMCT32, 
				0, 0, 0, 0);
		
		// Draw a solid, textured rectangle, with our texture on it (at z=1)
		pipeRectTexture(
			0, machine_def.y1_offset, 	// x1,y1
		   	0, 0, 				// u1,v1
		   	320, machine_def.y2_offset,	// x2,y2
		   	320, dph, 			// u2,v2
		   	10, 				// z
			 GS_SET_RGBA(0x80,0x80,0x80,0x80) 	// color
			); 		
		
		// Shade neocd display
		pipeRectFlat(0, 0, 320, machine_def.vdph, 11, GS_SET_RGBA(0, 0, 0, 64));
		pipeRectGouraud((96-16),((ypos[0]>>4)-5),GS_SET_RGBA(0x00,0x00, 0x20, 100), (320-96+16),((ypos[MAX_MENU_ITEM-1]>>4)+23), GS_SET_RGBA(0x00,0x00, 0x20, 100), 13);   	    	
		pipeRectLine((95-16), ((ypos[0]>>4)-5), (320-96+16), ((ypos[MAX_MENU_ITEM-1]>>4)+23), 14, GS_SET_RGBA(255, 255, 255, 128));

	
		TextOutC2(0<<4,320<<4,ypos[0]," - Resume - ",15);
		
		if (neocdSettings.renderFilter==0)
			TextOutC2(0<<4,320<<4,ypos[1],"Filter : Nearest",15);	
		else 
			TextOutC2(0<<4,320<<4,ypos[1],"Filter : Linear",15);	
			
		if (machine_def.vidsys == NTSC_MODE)
		   TextOutC2(0<<4,320<<4,ypos[2],"Fullscreen : Yes",15);
		else if (machine_def.y1_offset == 0)
		   TextOutC2(0<<4,320<<4,ypos[2],"Fullscreen : Yes",15);
		else
		  TextOutC2(0<<4,320<<4,ypos[2],"Fullscreen : No",15);
		
		if (machine_def.vidsys == NTSC_MODE)			
        	  TextOutC2(0<<4,320<<4,ypos[3],"Video : NTSC",15);
        	else
        	  TextOutC2(0<<4,320<<4,ypos[3],"Video : PAL",15);
			
		if (neocdSettings.frameskip==0)			
        	  TextOutC2(0<<4,320<<4,ypos[4],"Frameskip : Off",15);
        	else
        	  TextOutC2(0<<4,320<<4,ypos[4],"Frameskip : On",15);

		
		if (neocdSettings.soundOn==0)			
	        	TextOutC2(0<<4,320<<4,ypos[5],"Sound : Off",15);
		else
			TextOutC2(0<<4,320<<4,ypos[5],"Sound : On",15);
		
	    	if (neocdSettings.CDDAOn==0)			
			TextOutC2(0<<4,320<<4,ypos[6],"CDDA : Off",15);
		else
			TextOutC2(0<<4,320<<4,ypos[6],"CDDA : On",15);
		
		if (neocdSettings.region==REGION_JAPAN)
			TextOutC2(0<<4,320<<4,ypos[7],"Region : Japan",15);
		else if (neocdSettings.region==REGION_USA)
			TextOutC2(0<<4,320<<4,ypos[7],"Region : Usa",15);
		else //REGION_EUROPE
			TextOutC2(0<<4,320<<4,ypos[7],"Region : Europe",15);
	
		TextOutC2(0<<4,320<<4,ypos[8],"Enter Bios",15);
		
		TextOutC2(0<<4,320<<4,ypos[9],"Reset emulation",15);
	
		// draw the selected item highlighted
		pipeRectFlat((95-16), ypos[selection]>>4, (320-95+16), (ypos[selection] >>4 ) + 16, 17, GS_SET_RGBA(128, 255, 255, 40));


		// Flush the pipe, to draw the prims
		pipeFlush(); 

		// Indicate that we have finished drawing
		gsDrawBufferComplete();

		// if no new buffer is available for drawing, then wait until one is
		while(!(gsIsDrawBufferAvailable()));

		// once buffer is available then set the next display buffer and continue drawing
		gsSetNextDrawBuffer(); 

		// wait until pad can be read
		while (((ret=padGetState(PADDLE_1, 0)) != PAD_STATE_STABLE)&&(ret!=PAD_STATE_FINDCTP1)&&(ret != PAD_STATE_DISCONN)); // more error check ?
		padRead(0, 0, &pad1);
		pad1_data = 0xffff ^ pad1.btns;

		if((pad1.mode >> 4) == 0x07) 
		{
		   if(pad1.ljoy_v < 64) 
		      pad1_data |= PAD_UP;
		   else if(pad1.ljoy_v > 192) 
		      pad1_data |= PAD_DOWN;
		}
		new_pad = pad1_data & ~old_pad;
  		old_pad = pad1_data;

		
		if(pad1_data & PAD_SELECT) // Screen positioning
		{
             
			if((pad1_data & PAD_UP) && machine_def.dispy) machine_def.dispy--;
			if(pad1_data & PAD_DOWN) machine_def.dispy++;
			if((pad1_data & PAD_LEFT) && machine_def.dispx) machine_def.dispx--;
			if(pad1_data & PAD_RIGHT) machine_def.dispx++;
			
			if (machine_def.vidsys == PAL_MODE)
			{
				neocdSettings.dispXPAL=machine_def.dispx;
				neocdSettings.dispYPAL=machine_def.dispy;
			}	
			else
			{
				neocdSettings.dispXNTSC=machine_def.dispx;
				neocdSettings.dispYNTSC=machine_def.dispy;
			}

			gsSetDisplayPosition(machine_def.dispx, machine_def.dispy);
			
			settingsChanged=1;
			continue;
		}

     		if((new_pad & PAD_UP) && (selection > -1))
     		{
	     	  	if(selection>0)
           			selection--;
          		else selection=(MAX_MENU_ITEM-1); 
        	}
        
		if((new_pad & PAD_DOWN) && (selection < MAX_MENU_ITEM))
		{
		  	if(selection<(MAX_MENU_ITEM-1))          
            			selection++;
          		else selection=0;   
        	}


		if(new_pad & PAD_CROSS) 
		{
			if(selection == 0) break;
			
			if(selection == 1) // video filter
			{
				neocdSettings.renderFilter ^= 1;
				pipeSetFilterMethod(neocdSettings.renderFilter);
				settingsChanged=1;
			}	
		
			if(selection == 2) // fullscreen
			{
				if (machine_def.vidsys == PAL_MODE)
				{
			
					// option for PAL user only
			  		if (machine_def.y1_offset == 0) // set not fullscreen
			  		{ 
				  		machine_def.y1_offset = ((machine_def.vdph-dph)>>1) ;
						machine_def.y2_offset = (machine_def.vdph-((machine_def.vdph-dph)>>1));
						neocdSettings.fullscreen = 0;
			  		}	
			  		else // set fullscreen
			  		{ 
				  		machine_def.y1_offset = 0;
			  			machine_def.y2_offset = machine_def.vdph ;
			  			neocdSettings.fullscreen = 1;
			  		}	
			  		settingsChanged=1;
			  	}
			  	// NTSC is already fullscreen..
			
			}	
			
		
			if(selection == 3)  // video mode
			{
				if (machine_def.vidsys == PAL_MODE)	// swith to NTSC
				{
				   setVideoModeConfig(NTSC_MODE);	
				   gsSetDisplayMode(320, machine_def.vdph, 
			 		machine_def.dispx, machine_def.dispy, 
			 		GS_PSMCT32, 5, GS_TV_NTSC, GS_TV_NONINTERLACE,
			 		GS_ENABLE, GS_PSMZ32);	

				}	
				else 					// swith to PAL
				{
				   setVideoModeConfig(PAL_MODE);	
				   gsSetDisplayMode(320, machine_def.vdph, 
			 		machine_def.dispx, machine_def.dispy, 
			 		GS_PSMCT32, 5, GS_TV_PAL, GS_TV_NONINTERLACE,
			 		GS_ENABLE, GS_PSMZ32);
				}	
				
				
				// beurk..
				machine_def.y1_offset = ((machine_def.vdph-dph)>>1) ;
				machine_def.y2_offset = (machine_def.vdph-((machine_def.vdph-dph)>>1));
				
				// reset fullscreen flag
				neocdSettings.fullscreen = 0;
				
				// re-upload the last frame into VRAM
				
				pipeTextureUpload(
				gsGetTextureBufferBase(), 512,
				0, 0, (int)GS_PSMCT32, (const unsigned char*)&video_frame_buffer_ptr[0],
				BUF_WIDTH, BUF_HEIDTH);
				
				settingsChanged=1;
			}
			
			if(selection == 4)  // frameskip
			{
				neocdSettings.frameskip ^= 1;
				settingsChanged=1;
			}
				
			if(selection == 5)  // sound on/off
			{
				neocdSettings.soundOn ^= 1;
				settingsChanged=1;
			}
			
			if(selection == 6)
			{
				neocdSettings.CDDAOn ^= 1;
				settingsChanged=1;
				
				// stop CD playing
				if(!neocdSettings.CDDAOn)
				  cdda_shutdown();
			}
			
			if(selection == 7) // region
			{
				neocdSettings.region++;
				if (neocdSettings.region>2) neocdSettings.region=0;
				settingsChanged=1;
			}


			if(selection == 8) // enter Bios
			{
	          		enterBIOS();
             			break;
			}
			
			if(selection == 9) // Soft reset
			{
				// stop CDDA
				cdda_stop();
				// reset machine
	          		neogeo_reset();
             			break;
			}
		}
	}


	
	// Wait till X has stopped been pressed
	while(1) 
	{
		// wait till the pad can be read
		while (((ret=padGetState(PADDLE_1, 0)) != PAD_STATE_STABLE)&&(ret!=PAD_STATE_FINDCTP1)&&(ret != PAD_STATE_DISCONN)); // more error check ?
		padRead(0, 0, &pad1); // port, slot, buttons
		pad1_data = 0xffff ^ pad1.btns;
		if(!(pad1_data & PAD_CROSS)) break;
	}

	// save settings to MC
	if (settingsChanged)
	  mc_saveSettings();

	// Clear the screen (with ZBuffer Disabled)
	pipeSetZTestEnable(GS_DISABLE);
	pipeRectFlat(0,0,320,machine_def.vdph,0,GS_SET_RGBA(0,0,0,0x80));
	pipeSetZTestEnable(GS_ENABLE);
	
	if (neocdSettings.soundOn)
	   audsrv_set_volume(MAX_VOLUME);
	if(neocdSettings.CDDAOn)
	   cdda_resume();
}


//------------------------------------------------------------------
// display the splash/credit screen
void display_insertscreen(void)
{

   int cmpt=0;
   char text[100];
   static int scroll_delay = 0;
   int i,dec=0;
   
   //gsClearScreen();
   
   while(1) 
   {	
   	
   	cmpt++;
	if(cmpt>81)cmpt=0;
	
	//gsClearScreen();
	pipeSetZTestEnable(GS_DISABLE);
	pipeRectFlat(0,0,320,machine_def.vdph,0,GS_SET_RGBA(0,0,0,0x80));
	pipeSetZTestEnable(GS_ENABLE);
	
	//displayGPImageBuffer(splash);
	
	
	// Upload into the beginning of texture mem (with texture-buffer width set to 256)
	pipeTextureUpload(
		gsGetTextureBufferBase(), 256,
		0, 0, (int)GS_PSMCT32, (const unsigned char*)splash,
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
		   GS_SET_RGBA(0x80, 0x80, 0x80, 0x80));

		   //GS_SET_RGBA(255, 255, 255, 200) // color
		//); 

	//146
	
#ifdef BETA
	// Neocd/PS2 x.xb (c)Evilo '04 	
	sprintf(text,"%s %c.%cc %s", neocd_ps2, (char)(VERSION2_MAJOR+0x30), (char)(VERSION2_MINOR+0x30), copyright);
#else
	// Neocd/PS2 x.x (c)Evilo '04 	
	sprintf(text,"%s %c.%cc %s", neocd_ps2, (char)(VERSION2_MAJOR+0x30), (char)(VERSION2_MINOR+0x30), copyright);
#endif
	textCpixel(0,320,146, GS_SET_RGBA(0x80, 0x80, 0x80, 0x80)  ,0,0,12,text);
	
	//164
	textCpixel(0,320,164, GS_SET_RGBA(0x80, 0x80, 0x80, 0x80)   ,0,0,12,official_homepage);
	//172
	textCpixel(0,320,172, GS_SET_RGBA(0x80, 0x80, 0x80, 0x80)   ,0,0,12,neocd_ps2scene);
	

	//196
	if(cmpt<31)
		textCpixel(0,320,198,GS_SET_RGBA(0x80, 0x80, 0x80, 0x80)	   ,0,0,12," - Insert a CD and/or Press X to continue - ");
        else if (cmpt<64)
        	textCpixel(0,320,198,GS_SET_RGBA(0x80, 0x80, 0x80, (63 - cmpt)),0,0,12," - Insert a CD and/or Press X to continue - ");

	// 212
	for(i=0;i<sizeof(credit_scroll_text);i++)
           if((i*6-dec)<320 && (i*6-dec)>0) printch(i*6-dec,216+4,GS_SET_RGBA(104, 104, 104, 104),credit_scroll_text[i],0,0,12);	
     
     	if(--scroll_delay < 0) 
     	{
           dec++;
           if(dec>(6*sizeof(credit_scroll_text)+1))dec=0;
           scroll_delay = 0;
     	}   


	if (isButtonPressed (PAD_CROSS)) break;
		
			
	// Flush the pipe, to draw the prims
	pipeFlush(); 

	// Indicate that we have finished drawing
	gsDrawBufferComplete();

	// if no new buffer is available for drawing, then wait until one is
	while(!(gsIsDrawBufferAvailable()));

	// once buffer is available then set the next display buffer and continue drawing
	gsSetNextDrawBuffer(); 
    	
   } // end while  	

}

// display the loading screen
void display_loadingscreen(void)
{
	// deactivate bi-linear filtering
	pipeSetFilterMethod(GS_FILTER_NEAREST);
	
	// Clear the screen (with ZBuffer Disabled)
	pipeSetZTestEnable(GS_DISABLE);
	pipeRectFlat(0,0,320,machine_def.vdph,0,GS_SET_RGBA(0,0,0,0x80));
	pipeSetZTestEnable(GS_ENABLE);

	
	displayGPImageBuffer(loading);
	
	// Indicate that we have finished drawing
	gsDrawBufferComplete();

	// if no new buffer is available for drawing, then wait until one is
	while(!(gsIsDrawBufferAvailable()));

	// once buffer is available then set the next display buffer and continue drawing
	gsSetNextDrawBuffer(); 
	
	// reset current filter
	pipeSetFilterMethod(neocdSettings.renderFilter);
}

// display an error msg screen
void display_errorMessage(char *msg)
{
	// deactivate bi-linear filtering
	pipeSetFilterMethod(GS_FILTER_NEAREST);
	
	// Clear the screen (with ZBuffer Disabled)
	pipeSetZTestEnable(GS_DISABLE);
	pipeRectFlat(0,0,320,machine_def.vdph,0,GS_SET_RGBA(0,0,0,0x80));
	pipeSetZTestEnable(GS_ENABLE);
	
	pipeTextureUpload(
		gsGetTextureBufferBase(), 256,
		0, 0, (int)GS_PSMCT32, (const unsigned char*)&(loading),
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
		   256, 256, 			// u2,v2
		   10, 				// z
		   GS_SET_RGBA(0x80, 0x80, 0x80, 0x80) // color
		); 

	  
	pipeRectGouraud((56), 102,GS_SET_RGBA(204,0x00, 51, 100), (264), 122, GS_SET_RGBA(204,0x00, 51, 100), 11);
	pipeRectLine((56), 102, 			             (264), 122, 12, GS_SET_RGBA(255, 255, 255, 128));

	
	textCpixel(0,320,108, GS_SET_RGBA(0x80, 0x80, 0x80, 0x80)   ,0,0,13,msg);
	
	// Flush the pipe, to draw the prims
	pipeFlush(); 

	// Indicate that we have finished drawing
	gsDrawBufferComplete();

	// if no new buffer is available for drawing, then wait until one is
	while(!(gsIsDrawBufferAvailable()));

	// once buffer is available then set the next display buffer and continue drawing
	gsSetNextDrawBuffer(); 

}
