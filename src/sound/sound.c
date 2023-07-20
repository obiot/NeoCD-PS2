/*  gngeo a neogeo emulator
 *  Copyright (C) 2001 Peponas Mathieu
 *  Copyright (C) 2004-2005 Olivier "Evilo" Biot (PS2 Port)
 * 
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

  
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <kernel.h>
#include <audsrv.h> 

#include "fm.h"
#include "2610intf.h"
#include "sound.h"
#include "neocd.h"

//#define SOUND_THREAD	1

#define SAMPLE_RATE    12000


int16 *ps2_soundbuf[2] __attribute__((aligned(64)));

// 
#ifdef SOUND_THREAD
static int  sound_ThreadID = 0;
static void sound_Thread(void);
static char sound_ThreadStack[16*1024] __attribute__((aligned(64))); 
// imports
extern void * _gp; 
#endif
 
// init audio
int init_audio(void)
{
	
   struct audsrv_fmt_t format; 
   int err,ret; 
  
  
   // Init audsrv
   ret = audsrv_init();
   if (ret != 0)
   {
   	printf("sample: failed to initialize audsrv\n");
        printf("audsrv returned error string: %s\n", audsrv_get_error_string());
        return 1;
   }
   
   format.bits = 16;
   format.freq = SAMPLE_RATE;
   format.channels = 2;

   err = audsrv_set_format(&format);
   
   if (neocdSettings.soundOn)
   	audsrv_set_volume(MAX_VOLUME);
   else
        audsrv_set_volume(MIN_VOLUME);


   // Init FM emulation
   YM2610_sh_start(SAMPLE_RATE);
      
   
   // into SPR
   ps2_soundbuf[0] = (int16 *)sprAlloc(machine_def.snd_sample * 2); 
   ps2_soundbuf[1] = NULL; 

#ifdef SOUND_THREAD

   ee_thread_t th_attr;
   
   // setup sound thread
   printf("Sound Thread"); 
   th_attr.func = sound_Thread;
   th_attr.stack = sound_ThreadStack;
   th_attr.stack_size = sizeof(sound_ThreadStack);
   th_attr.gp_reg = &_gp;
   th_attr.initial_priority = 64;
   th_attr.option = 0;

   ret = CreateThread(&th_attr);
   if (ret < 0) {
       printf("init createThread failed %d\n", ret);
       return -1;
   }
   
   sound_ThreadID = ret;

   ret = StartThread(sound_ThreadID, 0);
   if (ret < 0) 
   {
   	printf("init startThread failed %d\n", ret);
        sound_ThreadID = 0;
        DeleteThread(sound_ThreadID);
        return -1;
   } 
   printf("...created with ID %d\n", sound_ThreadID); 
   
#endif
   
   printf("audio started\n");   
   
   return 0;
}

#ifdef SOUND_THREAD
// update sound for one frame
static void sound_Thread(void)
{
     printf ("Thread running\n");
     while(1)
     {
     	SleepThread(); 
     	printf("update sound\n");
     	YM2610UpdateOne((int16**)ps2_soundbuf, machine_def.snd_sample);
     
     	audsrv_wait_audio( machine_def.snd_sample << 2 );
     	audsrv_play_audio( (char*)ps2_soundbuf[0], machine_def.snd_sample << 2 );
     }
     ExitDeleteThread();
}
#endif

// update sound for one frame
void play_audio(void)
{

#ifdef SOUND_THREAD   
     WakeupThread(sound_ThreadID);

     //asm __volatile__("sync");
     asm __volatile__("ei"); 
#else
     YM2610UpdateOne(ps2_soundbuf, machine_def.snd_sample);
     
     audsrv_wait_audio( machine_def.snd_sample << 2 );
     audsrv_play_audio( (char*)ps2_soundbuf[0], machine_def.snd_sample << 2 );
#endif
     
}

void sound_toggle(void) 
{
	// pause Audio ...
	neocdSettings.soundOn ^= 1;
	
	if (neocdSettings.soundOn) 
	 audsrv_set_volume(MAX_VOLUME);
	else  
	 audsrv_set_volume(MIN_VOLUME);
}





