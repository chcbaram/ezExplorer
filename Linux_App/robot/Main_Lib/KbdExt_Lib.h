/*  serial port interface for Palm Portable Keyboard
 *  Copyright (C) 2003 
 *  Trap <CXL00145@nifty.com> (linux zaurus SL-C7x0 version)
 *
 *  Based on ser_code.c created by ...
 *    Dan Morris <dmorris@cs.brown.edu> (initial and windows version)
 *    Ralf Ackermann <rac@KOM.tu-darmstadt.de>
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
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *  2003.12.14 v1.1.0 : Performance Up.
 *    The weight of the value of the parameter (keyrep_first, keyrep_next)
 *    was changed. When using -d option, a default config file changed into
 *    ser_code_direct.ini automatically.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <libgen.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <termio.h>
#include <error.h>

#ifndef errno
extern int errno;
#endif

#include <asm/sharp_char.h>
#include <asm/sharp_keycode.h>
#include <linux/kbd_ll.h>



// Constants
#define	MYFLG_SHIFT	0
#define	MYFLG_CTRL	1
#define	MYFLG_FN	2
#define	MYFLG_CMD	3



#define RET_OK	         0
#define RET_ERROR	-1

#define	RTS_TIMEOUT	600

#define VERSION		"0.1"


#define KBD_EXT_DIRECT_MODE      0    // 게임들을 위한 다이렉트 모드
#define KBD_EXT_NOMAL_MODE       1    // 일반적인 키보드 모드


#define KBD_EXT_NONE_KEY_PRESS   -1;


#define KBD_EXT_MAX_KEY          128
#define KBD_EXT_MAX_META_STR     6
#define KBD_EXT_MAX_OPT_STR      2
#define KBD_EXT_MAX_META_KEY     (1<<KBD_EXT_MAX_META_STR)

#define KBD_EXT_PRESSED          1
#define KBE_EXT_RELEASED         0


#define KBD_EXT_OPT_KEY_FIRST    0
#define KBD_EXT_OPT_KEY_NEXT     1



typedef struct 
{
    char    *Key_Name;
    long     Key_Code;
} KBD_EXT_MAP_OBJ;




static int    KbdExt_Mode            =  KBD_EXT_NOMAL_MODE;	 // Direct 인지Normal 인지 	

static int    KbdExt_KeyRepeat       =  0;	
static int    KbdExt_KeyRepeat_First =  30;
static int    KbdExt_KeyNext         =  10;
static long   KbdExt_LastKey_Code    = KBD_EXT_NONE_KEY_PRESS;	

#ifdef IPK_PACK
static char  *KbdExt_KeyMap_D_Ptr   = "./KeyMap_Direct.ini";
static char  *KbdExt_KeyMap_N_Ptr   = "/opt/QtPalmtop/etc/KeyMap_Normal.ini";
#else
static char  *KbdExt_KeyMap_D_Ptr   = "./KeyMap_Direct.ini";
static char  *KbdExt_KeyMap_N_Ptr   = "./KeyMap_Normal.ini";
#endif

static char  *KbdExt_MetaStr_Left [ KBD_EXT_MAX_META_STR ] = { "lshift", "lctrl", "lfn", "lcmd","lopt","lalt" };
static char  *KbdExt_MetaStr_Right[ KBD_EXT_MAX_META_STR ] = { "rshift", "rctrl", "rfn", "rcmd","ropt","ralt" };
static char   KbdExt_MetaChr      [ KBD_EXT_MAX_META_STR ] = { 'S'     , 'C'    , 'F'  , 'M'   ,'O'   ,'A'   };
static char  *KbdExt_OptStr       [ KBD_EXT_MAX_OPT_STR  ] = { "keyrep_first",
                                                               "keyrep_next"
                                                             };

static int		KbdExt_MetaFlag;	
static char		KbdExt_Keys [ KBD_EXT_MAX_KEY ];		
static unsigned char	KbdExt_Metas[ KBD_EXT_MAX_KEY ];		


static KBD_EXT_MAP_OBJ	KbdExt_KeyMap[ KBD_EXT_MAX_META_KEY ][KBD_EXT_MAX_KEY];	




