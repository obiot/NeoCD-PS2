/*
 *  cdaudio.h - CDDA Player
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

#ifndef	CDAUDIO_H
#define CDAUDIO_H

//-- Exported Variables ------------------------------------------------------
//extern int	cdda_current_track;
//extern static volatile int 	cdda_playing;
//extern int	cdda_autoloop;

//-- Exported Functions ------------------------------------------------------
int		cdda_init(void);
void		cdda_play(int);
void		cdda_pause(void);
void		cdda_stop(void);
void		cdda_resume(void);
void		cdda_shutdown(void);
void		cdda_loop_check(void);
int 		cdda_get_disk_info(void);
void    	cdda_set_autoloop(int);


#endif /* CDAUDIO_H */

