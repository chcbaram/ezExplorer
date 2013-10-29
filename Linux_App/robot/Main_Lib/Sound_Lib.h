//----------------------------------------------------------------------------------------------
//            TITLE       : 시리얼 센서 라이브러리 헤더
//            WORK        :
//            DATE        : 2004. 2. 12
//            FILE        : Sen_Lib.h
//            Modified By : - IRSL -
//----------------------------------------------------------------------------------------------



#ifndef  _SOUND_LIB_H
#define  _SOUND_LIB_H
          


#ifdef   SOUND_LIB_LOCAL
#define  EXT_SOUND_LIB_DEF 
#else
#define  EXT_SOUND_LIB_DEF     extern
#endif

#ifdef   LIB_INCLUDE
#undef  EXT_SOUND_LIB_DEF
#define EXT_SOUND_LIB_DEF
#endif




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>



#include 	<fcntl.h>
#include 	<sys/ioctl.h>
#include 	<sys/soundcard.h>

#include        "sndfile.h"

#define	SIGNED_SIZEOF(x)	((int) sizeof (x))
#define	BUFFER_LEN			(2048)



EXT_SOUND_LIB_DEF int Sound_Stop_Flag;



/*------------------------------------------------------------------------------
**	Linux/OSS functions for playing a sound.
*/


EXT_SOUND_LIB_DEF int	linux_open_dsp_device (int channels, int srate) ;
EXT_SOUND_LIB_DEF int linux_play ( char *argv );

EXT_SOUND_LIB_DEF void Sound_Init(void);
EXT_SOUND_LIB_DEF int Sound_Play( char *File );
EXT_SOUND_LIB_DEF int Sound_Stop( void  );


#endif
