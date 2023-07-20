/*
 *  cdaudio.c - CDDA Player
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
 
//-- Include files -----------------------------------------------------------
#include "neocd.h"

#include <kernel.h> 

//-- Private Variables -------------------------------------------------------
static volatile int cdda_min_track;
static volatile int cdda_max_track;
static volatile int cdda_track_start;

//-- Public Variables --------------------------------------------------------
static volatile int cdda_current_track 	 = 0;
static volatile int cdda_replay_track 	 = 0;
static volatile int cdda_callback_enable = 0;
static volatile int cdda_autoloop 	 = 0;
static volatile int cdda_disabled	 = 0; 
static volatile int cdda_playing	 = 0;


// prototypes : defined in audsrv_rpc.c
int audsrv_on_cdda_stop(audsrv_callback_t cb, void *arg);

//----------------------------------------------------------------------------
static void cdda_stopped(void *arg)
{
	
	cdda_playing = 0;
	
	// restart same track
    	if ((cdda_autoloop)&&(cdda_callback_enable))
    	{
    	   cdda_replay_track = 1;
    	}
	
}
 

//----------------------------------------------------------------------------
int	cdda_init(void)
{
	
	cdda_min_track = cdda_max_track = 0;
	cdda_current_track = 0;
	cdda_replay_track = 0;
	cdda_playing = 0;
	cdda_track_start=0;
	cdda_disabled = 0;
	cdda_callback_enable = 0;
	
	printf("setting track end callback\n");
	audsrv_on_cdda_stop((void *)cdda_stopped, 0); 

	return	1;
}

//----------------------------------------------------------------------------
int cdda_get_disk_info(void)
{
    //int i;
    
    // no need to re-do a getInfo if already done -> avoid buzz/freeze ?
    if((neocdSettings.CDDAOn) && (cdda_max_track<=0))
    {
        cdda_min_track = 0;
     	cdda_max_track = audsrv_get_numtracks();
     	
     	// auto-disable if no CDDA tracks found
     	if (cdda_max_track <= 0) 
     	  cdda_disabled = 1;
     	else
          cdda_disabled = 0;

     	return 0;
    }
    return 1;
}


//----------------------------------------------------------------------------

void cdda_play(int track)
{

    
    if(cdda_disabled)
      return ;//1;
	
    if (cdda_playing && (cdda_current_track == track)) return ;//1; 
        
    
    if (cdda_playing)
      // stop playing track
      cdda_stop();
      
    cdda_replay_track = 0;
    audsrv_play_cd(track);
    audsrv_resume_cd();
    cdda_current_track = track; 
    cdda_playing = 1;
    cdda_callback_enable = 1;
    
    return;
}


//----------------------------------------------------------------------------
void cdda_pause(void)
{
    if ((cdda_disabled) || (cdda_playing==0))
      return;

    audsrv_pause_cd();
    cdda_playing = 0;

}

//----------------------------------------------------------------------------
void	cdda_resume(void)
{
    if ((cdda_disabled)||(cdda_playing==1))
      return;
      
    audsrv_resume_cd();
    cdda_playing = 1;

}

//----------------------------------------------------------------------------
void	cdda_stop(void)
{   
    if(cdda_disabled)
      return;

    cdda_callback_enable = 0;
    //audsrv_pause_cd();
    audsrv_stop_cd();
    cdda_playing = 0;
    cdda_current_track = 0;
    
    

}

//----------------------------------------------------------------------------
void cdda_shutdown(void)
{
    if(cdda_disabled)
      return;

    // stop playback    
    cdda_stop();
    // stop the cd spinning
    CDVD_Stop();
    return;
}

// set auto loop flag
//----------------------------------------------------------------------------
void cdda_set_autoloop(int auto_loop)
{
    cdda_autoloop = auto_loop;
}

// auto-loop check
//----------------------------------------------------------------------------
void cdda_loop_check(void) 
{
    if (cdda_replay_track == 1)
    {
	cdda_play(cdda_current_track);
    } 
}
