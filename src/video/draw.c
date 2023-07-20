/*
 *  draw.c - Video Hardware Emulation
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

#include <string.h>
#include "video.h"


extern unsigned char	*dda_y_skip;

unsigned int	fc=0;

//---------------------------------------------------------------------------- 
void video_draw_screen1() 
{ 
   register 	int count,y,i,offs;
   int		sx =0,sy =0,oy =0,my =0,zx = 1, rzy = 1; 
   int         	tileno,tileatr,t1,t2,t3; 
   int          dday=0,rzx=15,yskip=0;
   char        	fullmode=0; 


   for( y=224; y--; )
      //memset ((void*)video_line_ptr[y],video_paletteram_pc[4095], BUF_WIDTH * 2);
      memset ((void*)video_line_ptr[y],video_paletteram_pc[4095], 640);


   for (count=0;count< 0x300 ;count+=2) 
   { 
      t3 = *((unsigned short *)( &video_vidram[0x10000 + count] )); 
      t1 = *((unsigned short *)( &video_vidram[0x10400 + count] )); 
      
      // If this bit is set this new column is placed next to last one 
      if (t1 & 0x40)
      { 
         sx += (rzx + 1); 
         if ( sx >= 0x1F0 ) 
            sx -= 0x200; 

         // Get new zoom for this column 
         zx = (t3 >> 8) & 0x0F; 

         sy = oy; 
      } 
      else // nope it is a new block  
      {   
         // Sprite scaling 
         t2 = *((unsigned short *)( &video_vidram[0x10800 + count] )); 
         
         zx = (t3 >> 8) & 0x0F; 
         rzy = t3 & 0xff; 
	 	 
         sx = (t2 >> 7); 
         if ( sx >= 0x1F0 )
            sx -= 0x200; 	

                  
         // Number of tiles in this strip 
         my = t1 & 0x3f; 
         
         if (my == 0x20) 
            fullmode = 1; 
         else if (my >= 0x21) 
            fullmode = 2;   // most games use 0x21, but 
         else 
            fullmode = 0;   // Alpha Mission II uses 0x3f 
        
                  
         sy = 0x1F0 - (t1 >> 7); // 0x200
         
         if (sy > 0x100) sy -= 0x200;
          
         if (fullmode == 2 || (fullmode == 1 && rzy == 0xff)) 
         { 
            while (sy < -16) sy += (rzy + 1) << 1; //2 * (rzy + 1); 
         } 
         oy = sy; 
         
         if(my==0x21) my=0x20; 
         
         else if(rzy!=0xff && my!=0) 
            my = ((my<<4<<8)/(rzy+1)+15)>>4; //my = ((my*16*256)/(rzy+1)+15)/16;

         if(my>0x20) my=0x20; 
         

      } 

      rzx = zx; 
      
      // No point doing anything if tile strip is 0 
      if ((my==0)||(sx>=320)||(sx <-16)) // 320
         continue; 

      // Setup y zoom 
      if(rzy==255) 
         yskip=16; 
      else 
         dday=0;   // =256; NS990105 mslug fix 

      offs = count<<6; 
      

      // my holds the number of tiles in each vertical multisprite block 
      for( y=my; y--; )
      { 
      	
         tileno  = *((unsigned short *)(&video_vidram[offs])); 
         offs+=2; 
         tileatr = *((unsigned short *)(&video_vidram[offs])); 
         offs+=2; 
        

         if (tileatr&0x8) 
            tileno = (tileno&~7)|(neogeo_frame_counter&7); 
         else if (tileatr&0x4) 
            tileno = (tileno&~3)|(neogeo_frame_counter&3); 
            

         //  tileno &= 0x7FFF; 
         if (tileno>0x7FFF) 
            continue; 

         if (fullmode == 2 || (fullmode == 1 && rzy == 0xff)) 
         { 
            if (sy >= 248) sy -= (rzy + 1) << 1; //2 * (rzy + 1); 
         } 
         else if (fullmode == 1) 
         { 
            if (y == 0x10) sy -= (rzy + 1) << 1; //2 * (rzy + 1); 
         } 
         else if (sy > 0x110) sy -= 0x200;// NS990105 mslug2 fix
 
 
 	
 	 // test if sprite drawing is necessary        
         if (((tileatr>>8)||(tileno!=0))&&(sy < 224)&&(video_spr_usage[tileno] != 0))
         //if (((tileatr>>8)||(tileno!=0))&&(sy < 224)&&(sx >= -16))
         { 
	 	// setup yzoom
         	if(rzy!=255) 
	        { 
	            yskip=0; 
	            dda_y_skip[0]=0; 
	            for(i=0;i<16;i++) 
	            { 
	               dda_y_skip[i+1]=0; 
	               dday-=rzy+1; 
	               if(dday<=0) 
	               { 
	                  dday+=256;
	                  yskip++; 
	               } 
	               dda_y_skip[yskip]++; 
	            } 
	         } 
         
            	video_draw_spr( tileno, 
               			tileatr >> 8, 
               		   	tileatr & 0x01,
               		   	tileatr & 0x02, 
              			sx,sy,rzx,yskip); 
         } 

         sy +=yskip; 
      }  // for y 
   }  // for count 

   if (fc++ > neogeo_frame_counter_speed) 
   { 
      fc=0; // 3
      neogeo_frame_counter++;
   } 
   fc++; 
    
   video_draw_fix(); 
    
   // DRAW !!!!!!!
   blitter();


}
