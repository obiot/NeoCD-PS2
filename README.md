# NeoCD/PS2
![logo](https://github.com/obiot/NeoCD-PS2/assets/4033090/a77f76b4-cf20-4414-bf18-b2e50426fb89)

an open source NeoGeo CD emulator running for the Sony Playstation2™

# Introduction
NeoCD/PS2 is a open source NeoGeo CD emulator running on the Sony Playstation 2™. It is a port of NeoCD/SDL, the excellent NeoGeo CD emulator by [Foster](http://pacifi3d.retrogames.com/neocdsdl/). all SDL portions have been removed and replaced by PS2 "native" libraries (using PS2SDK).

![screen1](https://github.com/obiot/NeoCD-PS2/assets/4033090/b6772561-7ea8-40c0-9a73-34833bcfd4ae)![screen2](https://github.com/obiot/NeoCD-PS2/assets/4033090/a1592af4-9308-49f5-a0a0-37f591c37d36)
![screen3](https://github.com/obiot/NeoCD-PS2/assets/4033090/d7067a40-57f6-4cc1-830c-d0718845e945)![screen4](https://github.com/obiot/NeoCD-PS2/assets/4033090/55657937-5164-4c68-9153-2a3a93d66d38)

# Disclaimer
NeoCD/PS2 is not endorsed by SNK Playmore in any way and is produced without the use of any copyrighted material which belongs to Sony Computer Entertainment Inc, or SNK Playmore, or any other party. 

NeoCD/PS2 should only be used to play games that the user legally owns. NeoCD/PS2 must never be distributed with any copyrighted games or other material. Any breach of these terms is out of the authors control and is not at the authors consent.
NeoCD/PS2 is a free sofware. You may copy and distribute it as much as you like assuming that you respect the above conditions.

The NeoCD/PS2 source code is distributed under the terms of the GNU General Public License.

NeoCD/PS2 is not responsible of any possible damage to your PS2, use it at your own risk.

#Features
- Very Good compatibilty
- PAL/NTSC auto-detection & Selector
- Loads games directly from the cdrom
- 2 controllers support
- NGCD memory card support (use 45K on the MC)
- Stereo sound emulation
- CDDA support
- Ingame menu
- Zip support (bios only)

#Usage

## General Requirements

- Before running the emulator be sure that the "host/cd" contains the following tree :
|-- neocd folder (or CD root)
|- neocd.bin or bios.zip (ngcd bios, not provided)
|- neocd.elf
|- neocdps2.txt
|- system.cnf (if burned on a cd)
|- install.pbt \
|- instlst.pbt \_ C.C. Boot Manager Install Scripts
|- remove.pbt / Needed to install neocd on the BM
|- run.pbt /

Before you ask, if ANY file is MISSING : IT WON'T WORK !

- bios.zip must contain the bios file (named "neocd.bin"). if not found neocd will then look directly for the neocd.bin file. (uncompressed neocd bios size is 512kb and correct bios should have his CRC32 equal to 0xDF9DE490).
Note, that if you use the emulator from host, I recommend not using a zipped bios. As through the network loading a zip file takes more time (so booting is slower).

-> bios is NOT provided with the emulator, so you have to search it by your own or try to dump it from your NGCD.
- Load the emulator (neocd.elf) using your favorite way. If you burn the emulator be sure to use mode1/iso9660/NO multisession.
- On the splash/intro screen, when you see the "Insert a CD" text, insert a neogeo cd into your drive, wait a few seconds (the time for the drive to spin) and press (X) on the pad.
- If all goes well NeoCD/PS2 should detect the game and run it.

#Controls

## In-game controls:

PS2 <> NEOCD button mapping :
- Arrows = A/D-Pad
- Left stick = A/D-Pad
- Cross = Button A
- Circle = Button B
- Square = Button C
- Triangle = Button D
- Start = Start
- Select = Select
- L2 + R2 = Ingame Menu​

## In-game Menu:
- Resume : Go back to current game
- Filter : Video filter (Nearest or Linear). with certain games, Linear filter is needed to correct some effect caused by the fullscreen video option.
- Fullscreen : Set video to fullscreen (remove black border for PAL only)
- Video : Set video mode to PAL / NTSC
- Frameskip : Enable / Disable frameskip
- Sound : Turn sound on/off
- CDDA : Turn CDDA on/off
- Region : Change console region (you will need to reset the emulation to apply it)
- Enter Bios : needed if you want to format the memory card (you will need to restard the emulator to get out (!bug!))
- Soft Reset : Reset emulation

Additionaly by holding [Select] when in the ingame menu, you can change the screen position using the directional pad.
If changed, settings will be automatically saved to the memory card.
At start up, they will be automatically restored to the last saved value.

## Current Issues
- Sound is buggy ("echo" in MS, missing sfx in KOF '99, etc.. ) when framerate is not full (need frameskip).
- Timing issues
- Graphics glitches in some games
- It's not possible to exit from the bios (ps2 reset needed!)
- and probably others ...!

# Authors

NeoCD/PS2 is released under GNU license version 2,  Review the [LICENSE](LICENSE) files for further details.

The following people have contributed to the Neocd/Ps2 project:

Alias       | Real Name         |
------------|-------------------|
evilo	    | Olivier Biot      |


.. and many people from the ps2dev community with feedback and fixes :) 
