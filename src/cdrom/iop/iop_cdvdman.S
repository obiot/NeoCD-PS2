/*
 *  iop_cdvdman.S CDVD Manager Function
 *  taken from .irx files with symbol table.
 *  Copyright (C) 2002, A.Lee & Nicholas Van Veen
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


	.text
	.set	noreorder


/* ############################### CDVDMAN STUB ####### */
/* # Added by Sjeep, 24th June 2002                   # */

.local	cdvdman_stub
cdvdman_stub:
	.word	0x41e00000
	.word	0
	.word	0x00000101
	.ascii	"cdvdman\0"
	.align	2

	.globl	CdInit			# 004
CdInit:
	j	$31
	li	$0, 4

	.globl	CdStandby 		# 005
CdStandby:
	j	$31
	li	$0, 5

	.globl	CdRead			# 006
CdRead:
	j	$31
	li	$0, 6

	.globl	CdSeek			# 007
CdSeek:
	j	$31
	li	$0, 7

	.globl	CdGetError		# 008
CdGetError:
	j	$31
	li	$0, 8

	.globl	CdGetToc		# 009
CdGetToc:
	j	$31
	li	$0, 9

	.globl	CdSearchFile	# 010
CdSearchFile:
	j	$31
	li	$0, 10

	.globl	CdSync			# 011
CdSync:
	j	$31
	li	$0, 11

	.globl	CdGetDiskType	# 012
CdGetDiskType:
	j	$31
	li	$0, 12

	.globl	CdDiskReady		# 013
CdDiskReady:
	j	$31
	li	$0, 13

	.globl	CdTrayReq		# 014
CdTrayReq:
	j	$31
	li	$0, 14

	.globl	CdStop			# 015
CdStop:
	j	$31
	li	$0, 15

	.globl	CdPosToInt		# 016
CdPosToInt:
	j	$31
	li	$0, 16

	.globl	CdIntToPos		# 017
CdIntToPos:
	j	$31
	li	$0, 17

	.globl	CdCheckCmd		# 021
CdCheckCmd:
	j	$31
	li	$0, 21

	.globl	CdReadClock		# 024
CdReadClock:
	j	$31
	li	$0, 24

	.globl	CdStatus		# 028
CdStatus:
	j	$31
	li	$0, 28

	.globl	CdCallback		# 037
CdCallback:
	j	$31
	li	$0, 37

	.globl	CdPause			# 038
CdPause:
	j	#31
	li	$0, 38

	.globl	CdBreak			# 039
CdBreak:
	j	$31
	li	$0, 39

	.globl	CdGetReadPos	# 044
CdGetReadPos:
	j	$31
	li	$0, 44

	.globl	CdReadChain		# 066
CdReadChain:
	j	$31
	li	$0, 66

