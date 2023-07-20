#     _  ___  ___   ___  ___     ____    ___ ___ 
# |\  | |___ |   | |        |  /  ___|  |    ___|
# | \_| |___ |___| |___  ___| /  |   ___|   |___  Neocd/Ps2 Project.
#----------------------------------------------------------------------
# Copyright 2004 - Olivier "evilo" Biot <evilo13@gmail.com>
# Released under GNU license
# Review neocd/ps2 COPYING & LICENSE files for further details.
#---------------------------------------------------------------------- 
# Cygwin Env
# GCC 3.2.2
# PS2SDK 1.2
#---------------------------------------------------------------------- 
# Compilation Flags
# -DCPU68K_USE_MUSASHI   use musashi 68k core (working core)
# -DCPU68K_USE_C68K	 use stef 68k core (wip, not working yet)
#
# -DCPUZ80_MAMEZ80_CORE  use mame z80 core (working core)
# -DCPUZ80_CZ80_CORE	 use stef z80 core (wip, not working yet)
#
# -DBETA	 	 activate beta/debug stuff
#---------------------------------------------------------------------- 

EE_BIN = neocd.elf
	    
EE_OBJS =  neocd.o pd4990a.o \
	   cpu_68k/cpu68k.o \
	   cpu_z80/z80intrf.o cpu_z80/cz80/cz80.o cpu_z80/mamez80/z80.o \
	   misc/misc.o misc/timer.o \
	   input/input.o \
	   memory/memory.o memory/bios.o memory/sysmem.o \
	   save/mc.o save/ngcdIcn.o \
	   gui/menu.o gui/ps2print.o \
	   sound/sound.o sound/2610intf.o sound/ay8910.o \
	   sound/fm.o sound/ymdeltat.o \
	   sound/timer.o static/audsrvIrx.o static/freesdIrx.o \
	   video/video.o video/draw.o video/spr.o video/draw_fix.o video/video_util.o \
	   cdrom/cdrom.o static/cdvdIrx.o cdrom/cdvd_rpc.o cdaudio/cdaudio.o \
	   unzip/unzip.o unzip/zfile.o

EE_CFLAGS  += -funroll-loops -finline-functions -freorder-blocks -funsigned-char \
	      -fno-exceptions -mhard-float -EL -ffast-math -fstrict-aliasing \
	      -DINLINE="inline" -DLSB_FIRST  -DROM_PADMAN -DCPU68K_USE_MUSASHI -DCPUZ80_MAMEZ80_CORE

# -fno-strict-aliasing -> FOR CZ80

EE_INCS    += -I. -I$(PS2SDK)/ports/include/zlib -I$(PS2SDK)/sbv/include/ -I../gs/
	     
EE_LDFLAGS += -L$(PS2SDK)/sbv/lib -L$(PS2SDK)/ports/lib -L./cpu_68k -L./static
EE_LIBS    += -lpatches -lcpu_68k -lcdvdfs -lgslibse -laudsrv -lc -lm -lz -lcdvd -lmc -lpad -lgcc


# target
all:  build-iop $(EE_BIN)
	@ee-strip $(EE_BIN)
	@cp $(EE_BIN) ../bin/$(EE_BIN)

build-iop:
	@echo Building IRXs
	$(MAKE) -C cdrom/iop 
	$(MAKE) -C sound/audsrv

clean:
	@rm -f $(EE_BIN) $(EE_OBJS)
	$(MAKE) -C cdrom/iop clean 
	
reset :
	ps2client -h 192.168.1.110 reset

run :
	ps2client -h 192.168.1.110 execee host:$(EE_BIN)

# inline
static/freesdIrx.o: $(PS2SDK)/iop/irx/freesd.irx
	$(PS2SDK)/bin/bin2o.exe $< $@ freesdIrx
static/audsrvIrx.o: sound/audsrv/bin/audsrv.irx
	$(PS2SDK)/bin/bin2o.exe $< $@ audsrvIrx
static/cdvdIrx.o: static/cdvd.irx
	$(PS2SDK)/bin/bin2o.exe $< $@ cdvdIrx


# include
include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal

