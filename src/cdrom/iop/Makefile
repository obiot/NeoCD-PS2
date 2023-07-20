#     _  ___  ___   ___  ___     ____    ___ ___ 
# |\  | |___ |   | |        |  /  ___|  |    ___|
# | \_| |___ |___| |___  ___| /  |   ___|   |___  Neocd/Ps2 Project.
#----------------------------------------------------------------------
# Copyright 2001-2005.
# Copyright 2005 - Olivier "evilo" Biot <evilo13@gmail.com>
# Released under GNU license
# Review neocd/ps2 COPYING & LICENSE files for further details.
#---------------------------------------------------------------------- 
 
IOP_CFLAGS  += -Wall -fno-builtin-printf -I$(PS2SDK)/iop/include/ -I.
IOP_LDFLAGS += -G0 -s 

IOP_SRC_DIR = ./ 
IOP_BIN = ../../static/cdvd.irx

IOP_OBJS = cdvd_iop.o iop_cdvdman.o


all: $(IOP_OBJS) $(IOP_BIN) 

clean:
	rm -f $(IOP_BIN) $(IOP_OBJS)
	
	
include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.iopglobal 
include ../../Rules.make
include ../../Rules.release  
