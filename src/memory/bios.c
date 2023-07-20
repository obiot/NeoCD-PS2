/*
 *  bios.c - neocd/aes/mvs bios mngt
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

#include <stdio.h>
#include <string.h>


#include "neocd.h"
#include "bios.h"

// where the bios is stored
char	*neogeo_rom_memory __attribute__((aligned(64))) = NULL;
char	*neogeo_rom_fix_memory __attribute__((aligned(64))) = NULL;

// patch the NGCD Bios
int ngcdPatchBios()
{
	
	// Check BIOS validity
	printf("patching BIOS...");
	
	if (*((short*)(neogeo_rom_memory+0xA822)) != 0x4BF9)
	{
		printf("Fatal Error: Invalid BIOS file.\n");
		display_errorMessage("NEOCD.BIN is not a valid BIOS !");
		return -1;
	}
	printf(" OK\n");

	// Patch BIOS load files w/ now loading message/
	*((short*)(neogeo_rom_memory+0x552)) = 0xFABF;
	*((short*)(neogeo_rom_memory+0x554)) = 0x4E75;
	// Patch BIOS load files w/out now loading/
	*((short*)(neogeo_rom_memory+0x564)) = 0xFAC0;
	*((short*)(neogeo_rom_memory+0x566)) = 0x4E75;
	// Patch BIOS CDROM Check/
	*((short*)(neogeo_rom_memory+0xB040)) = 0x4E71;
	*((short*)(neogeo_rom_memory+0xB042)) = 0x4E71;
	
	// Patch BIOS upload command/
	//*((short*)(neogeo_rom_memory+0x546)) = 0xFAC1; //mslug
	//*((short*)(neogeo_rom_memory+0x548)) = 0x4E75;

	// Patch BIOS CDDA check/
	*((short*)(neogeo_rom_memory+0x56A)) = 0xFAC3;
	*((short*)(neogeo_rom_memory+0x56C)) = 0x4E75;

	// Full reset, please/
	*((short*)(neogeo_rom_memory+0xA87A)) = 0x4239;
	*((short*)(neogeo_rom_memory+0xA87C)) = 0x0010;
	*((short*)(neogeo_rom_memory+0xA87E)) = 0xFDAE;

	//Trap exceptions/
	*((short*)(neogeo_rom_memory+0xA5B6)) = 0x4AFC;
	
	//free(fixtmp);
	//fixtmp=NULL;
	printf("DONE!\n");
	return 0;

}

/*----------------------
 * patch the AES Bios
 * WIP !! 
 *----------------------*/
int aesPatchBios()
{
   
   /* Remove memory check for now */
   *((short*)(neogeo_rom_memory+(0x11b00))) = 0x4e71;
   *((short*)(neogeo_rom_memory+(0x11b02))) = 0x4e71;
   *((short*)(neogeo_rom_memory+(0x11b16))) = 0x4ef9;
   *((short*)(neogeo_rom_memory+(0x11b18))) = 0x00c1;
   
   if (neogeo_rom_memory[0x11b01] == 0xba)
   {

	/* standard bios */
	//neogeo_has_trackball = 0;
	*((short*)(neogeo_rom_memory+(0x11b1a))) = 0x1b6a;
   }		
   else 
   {
     	/* special bios with trackball support */
	//neogeo_has_trackball = 1;
	*((short*)(neogeo_rom_memory+(0x11b1a))) = 0x0c94;
   }	
   
   /* Patch bios rom, for Calendar errors */
   *((short*)(neogeo_rom_memory+(0x11c14))) = 0x4e71;
   *((short*)(neogeo_rom_memory+(0x11c16))) = 0x4e71;
   *((short*)(neogeo_rom_memory+(0x11c1c))) = 0x4e71;
   *((short*)(neogeo_rom_memory+(0x11c1e))) = 0x4e71;

   /* Rom internal checksum fails for now.. */
   *((short*)(neogeo_rom_memory+(0x11c62))) = 0x4e71;
   *((short*)(neogeo_rom_memory+(0x11c64))) = 0x4e71;
	
   printf("DONE!\n");
   return 0;
	
}



// load requested Bios into memory
int initMachineBios(char *path_prefix, int bios) 
{
   int fd, zfd;
   char romfile[256] __attribute__((aligned(64)));
   char *fix_rom;
   int fixlen;
   // flag to know if bios is loaded from a zip file or not
   int zipped_bios = 0;
   
   // free mem if was previously allocated
   if (neogeo_rom_memory != NULL)
   {
     free (neogeo_rom_memory);
     neogeo_rom_memory = NULL;
   }
     
   // (re)allocate it
   switch (bios)
   {
	case NCD_MACHINE :
	   printf("BIOS ROM (512kb) ... \n");
   	   neogeo_rom_memory = (char*)memalign(64, 0x80000);
   	   break;
   	 case AES_MACHINE : 
   	   printf("BIOS ROM (128kb) ... \n");
   	   neogeo_rom_memory = (char*)memalign(64, 0x20000);
   	   break;
   	 default :
		return -1;
   }
   if (neogeo_rom_memory==NULL)
	return -1;
   
     
   // Load BIOS
   printf("Loading BIOS...\n");
     	
   switch (bios)
   {
	case NCD_MACHINE :

		// Load NGCD BIOS file
  		
   		// try first to load first the zip file
   		strcpy (romfile, path_prefix);
   		strcat (romfile, ZIP_NAME); // "Zipped BIOS"
   		
   		zfd = zip_open (romfile);
   		
   		if (zfd<0)
   		{  
   		   printf ("%s file not found, loading neocd.bin\n",romfile);
		   zipped_bios = 0;
		   // restore direct path to bios
		   strcpy (romfile, path_prefix);
   		   strcat (romfile, NCD_BIOS); // "BIOS file"
		}
		else
		   zipped_bios = 1;
		
		if (zipped_bios)
		   fd = zopen(NCD_BIOS); 
		else
		   fd = fioOpen(romfile, O_RDONLY);
		   
		if (fd<0)
   		{  
		   printf("Fatal Error: Could not load %s\n",romfile);
		   display_errorMessage("BIOS file (neocd.bin) not found !");
		   return -1;
		}
		
		// read bios file
		
		if (zipped_bios)
		   zread(fd, neogeo_rom_memory, 0x80000);
		else
		   fioRead(fd, neogeo_rom_memory, 0x80000);
		
		if (zipped_bios)
		   zclose(fd); 
		else 
		   fioClose(fd);
		
		// close the zip file
		if (zipped_bios)
		   zip_close();
   		

		// at this point bios is loaded into neogeo_rom_memory		   		
   		fixlen = 0x10000;
   		
   		// fix data are in the NGCD BIOS @ 0x70000
   		fix_rom = &neogeo_rom_memory[0x70000];
   		
   		// free neogeo_rom_fix_memory (not used)
   		if (neogeo_rom_fix_memory!=NULL)
   		{
   		  free (neogeo_rom_fix_memory);
   		  neogeo_rom_fix_memory = NULL;
   		}
		
		break;
		
	case AES_MACHINE :
		// Load AES/MVS BIOS file
		strcat (romfile,"neo-geo.rom");
    		fd = fioOpen(romfile, O_RDONLY);
    		if (fd < 0 ) 
    		{
			/* try new bios */
			/* TODO: Load the bios accordingly to --country */
			strcpy (romfile, path_prefix);
			strcat (romfile,"sp-s2.sp1");
			fd = fioOpen(romfile, O_RDONLY);
			if (fd < 0 ) 
			{
	    			printf("Can't find sp-s2.sp1 or neo-geo.rom\n");
	    			display_errorMessage("NEOGEO BIOS file not found !");
	    			return -1;
			}
    		}
    		fioRead(fd, neogeo_rom_memory, 0x20000);
    		fioClose(fd); 
    		
    		// allocate fix memory
   		if (neogeo_rom_fix_memory == NULL)
   		{
   			printf("BIOS FIX ROM (128kb) ... \n");
   			neogeo_rom_fix_memory = (char*)memalign(64, 0x20000);
   			if (neogeo_rom_fix_memory==NULL)
   			return -1;
   		}
    		
    		// load fix file
		strcpy (romfile, path_prefix);
		strcat (romfile,"ng-sfix.rom");
		fd = fioOpen(romfile, O_RDONLY);
		if (fd < 0 ) 
		{
			/* try new bios */
			strcpy (romfile, path_prefix);
			strcat (romfile,"sfix.sfx");
			fd = fioOpen(romfile, O_RDONLY);
			if (fd < 0 ) 
			{
		   		printf("Can't find fix file\n");
		   		display_errorMessage("FIX file not found !");
				return -1;
			}
		}
		fioRead(fd, neogeo_rom_fix_memory, 0x20000);
		fioClose(fd); 
		
		fixlen = 0x20000;
   		fix_rom = neogeo_rom_fix_memory;
   		
		break;
		
	default :
		return -1;
   }
  
   // convert bios fix char
   convert_all_char(fix_rom, fixlen, rom_fix_usage); 
   
   // patch bios if necessary
   switch (bios)
   {
	case NCD_MACHINE :
		return ngcdPatchBios();
	case AES_MACHINE :
		return aesPatchBios();
	default :
		return -1;
   }
   // done !
     
}

