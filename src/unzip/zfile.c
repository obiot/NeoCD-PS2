/*
 *  zfile.c - zip file io function 
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
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "unzip.h"

#define	PATH_MAX	256
static unzFile unzfile;
static char basedir[PATH_MAX];
char* basedirend;

int zip_open(const char *name)
{
	unzfile = unzOpen(name);
	if (unzfile) return (int)unzfile;

	strcpy(basedir,name);
	basedirend = basedir + strlen(basedir);
	*basedirend++='/';
	return -1;
}

int zip_close(void)
{
	if (unzfile) {
		unzClose(unzfile);
		unzfile = NULL;
	}
	return 1;
}

int zopen(const char *filename)
{
	if (unzfile==NULL) {
		strcpy(basedirend,filename);
		return open(basedir,O_RDONLY);
	}

	int ret = unzLocateFile(unzfile,filename,0);
	if (ret!=UNZ_OK) return -1;
	ret = unzOpenCurrentFile(unzfile);
	if (ret!=UNZ_OK) return -1;
	return (int)unzfile;
}

int zread(int fd,void * buf,unsigned size)
{
	if (unzfile == NULL)
		return read(fd,buf,size);
	return unzReadCurrentFile(unzfile,buf,size);
}

int zclose(int fd)
{
	if (unzfile == NULL) {
		close(fd);
		return 0;
	}
	return unzCloseCurrentFile(unzfile);
}

