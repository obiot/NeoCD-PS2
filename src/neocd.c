/*
 *  neocd.c - Main file
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

//-- Include Files -----------------------------------------------------------


#include <kernel.h>
#include <fileio.h> 
#include <stdio.h>
#include <string.h>
#include <sifcmd.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <malloc.h>
#include <iopcontrol.h>
#include <iopheap.h>
#include <libcdvd.h>
#include <libmc.h>
#include <sbv_patches.h>


#include "neocd.h"
#include "video/video.h"
#include "sound/sound.h"
#include "save/mc.h"
#include "static/patch.h"
#include "misc/timer.h"




//-- Global Variables --------------------------------------------------------

int		neogeo_ipl_done = 0;
unsigned char	config_game_name[128]  __attribute__((aligned(64)));
char 		path_prefix[256]  __attribute__((aligned(64))) = "cdfs:\\"; 

//uint32	neocd_time; /* next time marker */

int 		boot_mode = BOOT_UNKNOW; // by defaut
int 		game_boot_mode = BOOT_HOST; // by defaut

static int frameskip_counter = 0;

volatile int frame_counter = 0;

int fps; 
extern unsigned vsync_freq; 
   
static unsigned lastime, mytime =0, elapsed, mycounter; 

// default emulator settings
struct_neocdSettings neocdSettings  = 
{
		(VERSION2_MAJOR * 10 + VERSION2_MINOR),
		REGION_USA,	// Current region = USA
		0,		// Filter: Nearest
		1,		// SOUND: ON
#ifdef BETA
		1, 		// SHOW_FPS: ON
#else
		0, 		// SHOW_FPS: OFF
#endif
		0,		// frameskip
		1,		// CDDA: ON
		1,		// SAVE: ON
		83, 40,		// PAL x/y offset
		83, 25,		// NTSC x/y offset
		0,		// FULLSCREEN
		0,0,0		// RFU
}; 

// default runtime machine settings
// won't be saved into the mc
struct_machine machine_def = { 0,  0,   0,    0,
			       0,  0,   0,    0,
			       0,  0,   0,
			       NCD_MACHINE  // default machine type
			     }; // initialized later


//-- 68K Core related stuff --------------------------------------------------
int	mame_debug = 0;
int	previouspc = 0;


//-- Function Prototypes -----------------------------------------------------
void	neogeo_init(void);
void	neogeo_reset(void);
void	neogeo_hreset(void);
void	MC68000_Cause_Interrupt(int);
void	neogeo_exception(void);
void	neogeo_run(void);
void	draw_main(void);
void	neogeo_quit(void);
void	not_implemented(void);
void	neogeo_machine_settings(void);
void	neogeo_debug_mode(void);
void	neogeo_cdda_check(void);
void	neogeo_cdda_control(void);
void	neogeo_do_cdda( int command, int trck_number_bcd);
void	neogeo_read_gamename(void);

//----------------------------------------------------------------------------
int	main(int argc, char* argv[])
{
	int	result;
	
	
	// Displays version number, date and time
	printf(VERSION1);
	printf("%d.%d\n",VERSION2_MAJOR,VERSION2_MINOR);
	printf(VERSION3);
	printf(AUTHOR);
	
	// detect host : strip elf from path
	if (argc == 0)  // argc == 0 usually means naplink..
    	{
	        strcpy (path_prefix,"host:");
    	}
	else if (argc>=1)
	{
	    char *p;
	    if ((p = strrchr(argv[0], '/'))!=NULL) {
	      snprintf(path_prefix, sizeof(path_prefix), "%s", argv[0]);
	      p = strrchr(path_prefix, '/');
	      if (p!=NULL)
	        p[1]='\0';
	    } else if ((p = strrchr(argv[0], '\\'))!=NULL) {
	      snprintf(path_prefix, sizeof(path_prefix), "%s", argv[0]);
	      p = strrchr(path_prefix, '\\');
	      if (p!=NULL)
	        p[1]='\0';
	    } else if ((p = strchr(argv[0], ':'))!=NULL) {
	      snprintf(path_prefix, sizeof(path_prefix), "%s", argv[0]);
	      p = strchr(path_prefix, ':');
	      if (p!=NULL)
	        p[1]='\0';
	    }
	}
	
  	if (!strncmp(path_prefix, "cdrom", strlen("cdrom"))) 
  	{
        	printf("Booting from cd\n");
        	boot_mode = BOOT_CD;
        	// replace cdrom path with cdfs -> libcdvd
        	strcpy (path_prefix,"cdfs:\\");
        } else if(!strncmp(path_prefix, "mc", strlen("mc"))) {
        	printf("Booting from mc\n");
        	boot_mode = BOOT_MC;
    	} else if(!strncmp(path_prefix, "host", strlen("host"))) {
	        printf("Booting from host\n");
	        boot_mode = BOOT_HO;
	}  else if(!strncmp(path_prefix, "hdd", strlen("hdd"))) { //exist?
	        printf("Booting from hdd\n");
	        boot_mode = BOOT_HD;
	}  else if(!strncmp(path_prefix, "pfs", strlen("pfs"))) {
	        printf("Booting from hdd\n");
	        boot_mode = BOOT_HD;
    	}	

	//if we don't boot from host, reset the IOP

	// reset the IOP (no dev/debug)
	/*
	if (boot_mode != BOOT_HO)
	{
		SifInitRpc(0);
   		SifExitIopHeap();
   		SifLoadFileExit();
   		SifExitRpc();
   		SifIopReset("rom0:UDNL rom0:EELOADCNF",0);
		while (!SifIopSync()) ;
		// is cdvdman irx needed after that ?
	}
	*/

	SifInitRpc(0);
	printf("rpc init\n");
  	
  	// load modules
  	loadModules();  	
  	 	
  	// init CDROM
  	CDVD_Init();  
  	// init MC
  	mcInit(MC_TYPE_MC);
  	//init fio
	fioInit();
  	  		
	// Video init
	printf("\n");
	printf("Initializing video.... ");
	result = video_init();
	if (result==-1) 
	{
		printf("failed !\n");
		return 0;
	}
	// display the loading screen
	display_loadingscreen();
	
	if (result==PAL_MODE)
	   printf("PAL MODE SET\n");
	if (result==NTSC_MODE)
	   printf("NTSC MODE SET\n");
	
	// Initialize Memory Mapping
	if (initialize_memmap(machine_def.machine_type)==-1)
	{
		printf("failed !\n");
		return -1;
    	}
	
	// load BIOS
	if (initMachineBios(path_prefix, machine_def.machine_type)==-1)
	{
		printf("failed !\n");
		return -1;
    	}
    	
	// Init save
	printf("checking save on MC0...\n");
	mc_initSave();
	
	printf("checking neocd settings...\n");
	mc_readSettings();


	// Initialise input
	printf("Initialize input...\n");
	input_init();

	// stop the cd spinning
	CDVD_Stop();
	// display splash/credit screen before continuing
	display_insertscreen();

	// display loading screen
	display_loadingscreen();
	
	// Initialize CD-ROM
	printf("Initialize cdrom...\n");
	game_boot_mode = cdrom_init1();
	
	// Sound init
	init_audio();
	
	// Cdda init
	cdda_init();
		
	// timer init
	timer_init();

	// Initialize everything
	neogeo_init();
	pd4990a_init();
	neogeo_run();

	return 0;

}

//----------------------------------------------------------------------------
void	neogeo_init(void)
{
 	printf("Reset M68K...\n");
	M68K_Init();
}

//----------------------------------------------------------------------------
void	neogeo_hreset(void)
{
	printf("\nNEOGEO Hard Reset...\n");
	
	
	if (machine_def.machine_type == NCD_MACHINE)
	{
		// read game name
		neogeo_read_gamename();
		printf ("Game Name : %s\n",config_game_name);
	
		// Special patch for Samurai Spirits RPG
		if (strcmp(config_game_name, "TEST PROGRAM USA") == 0)
		{
	  		strcpy(config_game_name, "SAMURAI SPIRITS RPG");
			printf ("Patching SAMURAI SPIRITS RPG....");
			memcpy(neogeo_prg_memory + 0x132000, patch, PATCH_LEN); 
			swab(neogeo_prg_memory + 0x132000, neogeo_prg_memory + 0x132000, PATCH_LEN);
			printf ("DONE \n");
	
		}
	}
	
	// First time init
	printf("m68K reset...\n");
	
	 // First time init 
   	M68K_Reset();
   	
   	M68K_SetPC(0xc0a822);
   	M68K_SetSR(0x2700);
   	M68K_SetAReg(7, 0x10F300);
   	M68K_SetMSP(0x10F300);
   	M68K_SetUSP(0x10F300);
    
	m68k_write_memory_32(0x10F6EE, m68k_read_memory_32(0x68L)); // $68 *must* be copied at 10F6EE
	if (m68k_read_memory_8(0x107)&0x7E)
	{
		if (m68k_read_memory_16(0x13A))
		{
			m68k_write_memory_32(0x10F6EA, (m68k_read_memory_16(0x13A)<<1) + 0xE00000);
		}
		else
		{
			m68k_write_memory_32(0x10F6EA, 0);
			m68k_write_memory_8(0x00013B, 0x01);
		}
	}
	else m68k_write_memory_32(0x10F6EA, 0xE1FDF0);

	if (machine_def.machine_type == NCD_MACHINE)	
	{
   	
     	    	// write region
	     	m68k_write_memory_8(0x10FD83,neocdSettings.region);
	     
	     	// reset CDDA Tracks
	     	//cdda_current_track = 0;
	     	cdda_get_disk_info();	     
	}	
	else // AES & MVS
	{
		// write region
	     	*((short*)(neogeo_rom_memory+(0x0400))) = neocdSettings.region;
	}	
	
	printf("init z80...\n");
	_z80_init();
	printf("all done...\n");
 
}	

//----------------------------------------------------------------------------
void neogeo_reset(void)
{

	printf("NEOGEO Soft Reset...\n");
	
		
	M68K_Reset();
	
	
   		M68K_SetPC(0x122);
   		M68K_SetSR(0x2700);
   		M68K_SetAReg(7, 0x10F300);
   		M68K_SetMSP(0x10F300);
   		M68K_SetUSP(0x10F300);


		m68k_write_memory_8(0x10FD80, 0x82);
		m68k_write_memory_8(0x10FDAF, 0x01);
		m68k_write_memory_8(0x10FEE1, 0x0A);
		m68k_write_memory_8(0x10F675, 0x01);
		m68k_write_memory_8(0x10FEBF, 0x00);
		m68k_write_memory_32(0x10FDB6, 0);
		m68k_write_memory_32(0x10FDBA, 0);
	
	// MACHINE SPECIFIC settings
	if (machine_def.machine_type == NCD_MACHINE)	
	{
		// Set System Region
 	   	m68k_write_memory_8(0x10FD83,neocdSettings.region);
	   	//cdda_current_track = 0;
	}   
	else // AES & MVS
	{
		// Set System Region
	   	*((short*)(neogeo_rom_memory+(0x0400))) = neocdSettings.region;
	}

	printf("init z80...\n");

	_z80_init();
	printf("done...\n");

}

//----------------------------------------------------------------------------
void	neogeo_exception(void)
{
	printf("NEOGEO: Exception Trapped at %08x !\n", previouspc);
	exit(0);
}	

//----------------------------------------------------------------------------
void MC68000_Cause_Interrupt(int level)
{
	if (level >= 1 && level <= 7)
	  M68K_SetIRQ(level);

}

//----------------------------------------------------------------------------
void	neogeo_exit(void)
{
	printf("NEOGEO: Exit requested by software...\n");
	exit(0);
}

//----------------------------------------------------------------------------
void	neogeo_run(void)
{
 
	//uint32 now;

    	u16	i;

	printf("START EMULATION...\n");
	
	// If IPL.TXT not loaded, load it !
	if (!neogeo_ipl_done)
	{
		// Display Title
		// completely useless !!
		//cdrom_load_title();

		// Process IPL.TXT
		if (!cdrom_process_ipl())
		{
			printf("Error: Error while processing IPL.TXT.\n");
			return;
		}
		
		// copy game vectors
		memcpy(neogeo_game_vectors, neogeo_prg_memory, 0x80 );
		// Reset everything
		neogeo_ipl_done = 1;
		neogeo_hreset();
	}

		
	// Main loop
	my_timer();

	for(;;)
	{
		// Time management
		 lastime = mytime;
		 mytime  = timer_gettime();  
		 elapsed = mytime-lastime;
		 if (elapsed>0xffff)
		   elapsed = elapsed-0xffff0000; 
		 mycounter += elapsed;
		 if(mycounter>=vsync_freq)
		 {
		    fps=frame_counter;
		    // estimated framerate with frameskip
		    if (neocdSettings.frameskip)
		    	fps<<=1;
		    if (fps>machine_def.fps_rate)
		      fps = machine_def.fps_rate;
		      
		    frame_counter=0;
		    mycounter=0;   
		 }       
		 
		if (neocdSettings.soundOn || neocdSettings.CDDAOn)
		{
			
		    // One-vbl timeslice
		    #ifdef CPUZ80_CZ80_CORE
		    _z80raise(0);
		    #endif
		    
		    for( i=256; i--; ) // nb interlace
		    {
			      _z80exec(machine_def.z80_cycles_slice); //z80_cycles
			      my_timer();   
		    }
		}

		// One-vbl timeslice
		M68K_Exec(machine_def.m68k_cycles);
      		// raise irq
		M68K_SetIRQ(2);
		
		// update pd4990a
		pd4990a_addretrace();
		
		// check the watchdog
		if (watchdog_counter > 0) {
		    if (--watchdog_counter == 0) {
			printf("reset caused by the watchdog\n");
			neogeo_reset();
		    }
		}
		
		// check for memcard writes
		if (memcard_write > 0) {
		   memcard_write--;
		   if(memcard_write==0)
		   {
		     // write memory card here
		     printf("writing to memory card\n");
		     mc_writeSave();
		     // if you need to keep file up to date
		   }
		}

		// update audio buffer
		if (neocdSettings.soundOn)
		  play_audio();
		
		if (++frameskip_counter > neocdSettings.frameskip) 
   		{
			// Call display routine
			video_draw_screen1();
			frameskip_counter = 0;
    		}  


		// Update paddles
		processEvents();
					
		// Check if there are 
		// pending commands for CDDA
		if (neocdSettings.CDDAOn)
		{
		   neogeo_cdda_check();
		   cdda_loop_check();
		}

	} // end loop

	// Stop CDDA
	cdda_stop();
		
	return;
 
}

//----------------------------------------------------------------------------
// This is a really dirty hack to make SAMURAI SPIRITS RPG work
void inline neogeo_prio_switch(void)
{
   return;
   /*
   // this shouldn't be needed anymore ?
   if (M68K_GetDReg(7) == 0xFFFF)
      return; 
    
   if (M68K_GetDReg(7) == 9 &&
       M68K_GetAReg(3) == 0x10DED9 &&
      (M68K_GetAReg(2) == 0x1081d0 ||
      (M68K_GetAReg(2)&0xFFF000) == 0x102000)) {
      neogeo_prio_mode = 0; 
      return; 
   } 
    
   if (M68K_GetDReg(7) == 8 &&
       M68K_GetAReg(3) == 0x10DEC7 &&
      M68K_GetAReg(2) == 0x102900) {
      neogeo_prio_mode = 0; 
      return; 
   } 
    
   if (M68K_GetAReg(7) == 0x10F29C)
   { 
      if ((M68K_GetDReg(4)&0x4010) == 0x4010)
      { 
         neogeo_prio_mode = 0; 
         return; 
      } 
       
      neogeo_prio_mode = 1; 
   } 
   else 
   { 
      if (M68K_GetAReg(3) == 0x5140)
      { 
         neogeo_prio_mode = 1; 
         return; 
      } 

      if ( (M68K_GetAReg(3)&~0xF) == (M68K_GetAReg(4)&~0xF) )
         neogeo_prio_mode = 1; 
      else 
         neogeo_prio_mode = 0; 
   } 
   */

}

//----------------------------------------------------------------------------
void not_implemented(void)
{
		printf("Error: This function isn't implemented.\n");
}

//----------------------------------------------------------------------------
void neogeo_quit(void)
{
		exit(0);
}

//----------------------------------------------------------------------------
void neogeo_cdda_check(void)
{
  
	int		offset;
	
	offset = m68k_read_memory_32(0x10F6EA);
	if (offset < 0xE00000)	// Invalid addr
		return;

	offset -= 0xE00000;
	offset >>= 1;

	neogeo_do_cdda(subcpu_memspace[offset&0xFFFF], subcpu_memspace[(offset+1)&0xFFFF]);

}

//----------------------------------------------------------------------------
void inline neogeo_cdda_control(void)
{
    neogeo_do_cdda( (M68K_GetDReg(0)>>8)&0xFF,
                     M68K_GetDReg(0)&0xFF );
}

//----------------------------------------------------------------------------
void  neogeo_do_cdda( int command, int track_number_bcd)
{

	int		track_number;
	int		offset;

	if ((command == 0)&&(track_number_bcd == 0))
		return;

	m68k_write_memory_8(0x10F64B, track_number_bcd);
	m68k_write_memory_8(0x10F6F8, track_number_bcd);
	m68k_write_memory_8(0x10F6F7, command);
	m68k_write_memory_8(0x10F6F6, command);

	offset = m68k_read_memory_32(0x10F6EA);

	if (offset)
	{
		offset -= 0xE00000;
		offset >>= 1;

		m68k_write_memory_8(0x10F678, 1);

		subcpu_memspace[offset&0xFFFF] = 0;
		subcpu_memspace[(offset+1)&0xFFFF] = 0;

	}
	
	if (neocdSettings.CDDAOn)
	{
#ifdef BETA
	   printf ("CDDA command : %d\n", command);
#endif
	   switch( command )
	   {
		case	0:
		case	1:
		case	5:
		case	4:
		case	3:
		case	7:
			track_number = ((track_number_bcd>>4)*10) + (track_number_bcd&0x0F);
#ifdef BETA			
			printf ("track_number : %d\n",track_number);
#endif
			if (track_number == 0)//&&(!cdda_playing)) -> moved to audio.c
			{
				cdda_resume();
			}
			else if ((track_number>1)&&(track_number<99))
			{
				cdda_play(track_number);
				//cdda_autoloop = !(command&1);
				cdda_set_autoloop (!(command&1));
			}
			break;
		case	6:
		case	2:
			//if (cdda_playing) -> moved to cdaudio.c
			//{
				cdda_pause();
			//}
			break;
		default : break;
	    }
        }
}
//----------------------------------------------------------------------------
void neogeo_read_gamename(void)
{

	unsigned char	*Ptr;
	int		temp;

	Ptr = neogeo_prg_memory + m68k_read_memory_32(0x11A);
	swab(Ptr, config_game_name, 80);
	for(temp=0;temp<80;temp++) {
		if ((!isprint(config_game_name[temp]))||(config_game_name[temp]==0xFF)||
		((config_game_name[temp]==0x20)&&(config_game_name[temp+1]==0x20)))
		{
			config_game_name[temp]=0;
			break;
		}
	}

}

/*
 * loadModules()
 */
void loadModules(void)
{
    int ret;
        
    sbv_patch_enable_lmb();
    sbv_patch_disable_prefix_check();
        
    SifLoadModule("rom0:SIO2MAN", 0, NULL);
    SifLoadModule("rom0:PADMAN", 0, NULL);
    SifLoadModule("rom0:MCMAN", 0, NULL);  	
    SifLoadModule("rom0:MCSERV", 0, NULL); 
    
    //load cdvd irx
    SifExecModuleBuffer(cdvdIrx_start, cdvdIrx_size, 0, NULL, &ret);
    if (ret < 0)
    {
	printf("Failed to load module: libcdvd.irx \n");    
    }
    // load freesd (libsd replacement)
    SifExecModuleBuffer(freesdIrx_start, freesdIrx_size, 0, NULL, &ret);
    if (ret < 0)
    {
	printf("Failed to load module: freesd.irx \n");    
    }
    //load gawd audsrv irx    
    SifExecModuleBuffer(audsrvIrx_start, audsrvIrx_size, 0, NULL, &ret);
    if (ret < 0)
    {
	printf("Failed to load module: audsrv.irx \n");    
    }
}
