//----------------------------------------------------------------------------------------------
//            TITLE       : 사운드 라이브러리
//            WORK        :
//            DATE        : 2004. 11. 29
//            FILE        : Thread_Lib.c
//            Modified By : Hanchol Cho     - IRSL -
//----------------------------------------------------------------------------------------------


//----- 헤더파일 열기
//
#ifndef   LIB_INCLUDE
#define  SOUND_LIB_LOCAL  
#include "Sound_Lib.h"
#endif






//----------------------------------------------------------------------------------------------
//            TITLE   : linux_play
//
//            WORK    : 
//
//            DATE    : 2004. 11. 29
//----------------------------------------------------------------------------------------------
void Sound_Init(void)
{
	Sound_Stop_Flag = 0;	
}	





//----------------------------------------------------------------------------------------------
//            TITLE   : Sound_Play
//
//            WORK    : 
//
//            DATE    : 2004. 11. 29
//----------------------------------------------------------------------------------------------
int Sound_Play( char *File )
{
	int Ret;
		
	Sound_Stop_Flag = 0;
	Ret = linux_play ( File );   // 사운트 파일 플레이
	Sound_Stop_Flag = 0;	
	return Ret;	
}	



//----------------------------------------------------------------------------------------------
//            TITLE   : Sound_Stop
//
//            WORK    : 
//
//            DATE    : 2004. 11. 29
//----------------------------------------------------------------------------------------------
int Sound_Stop( void  )
{
	int Ret = 0;
		
	Sound_Stop_Flag = 1;
	
	return Ret;	
}	









//----------------------------------------------------------------------------------------------
//            TITLE   : linux_play
//
//            WORK    : 
//
//            DATE    : 2004. 11. 29
//----------------------------------------------------------------------------------------------
int linux_play ( char *argv )
{	static short buffer [BUFFER_LEN] ;
	SNDFILE *sndfile ;
	SF_INFO sfinfo ;
	int		k, audio_device, readcount, subformat ;

	memset (&sfinfo, 0, sizeof (sfinfo)) ;

	//printf ("Playing %s\n", argv [k]) ;
		
	if (! (sndfile = sf_open (argv, SFM_READ, &sfinfo)))
	{	
		puts (sf_strerror (NULL)) ;
		return -1;
	} 

	if (sfinfo.channels < 1 || sfinfo.channels > 2)
	{	
		printf ("Error : channels = %d.\n", sfinfo.channels) ;
		return -2;
	} 

	audio_device = linux_open_dsp_device (sfinfo.channels, sfinfo.samplerate) ;

	subformat = sfinfo.format & SF_FORMAT_SUBMASK ;

	if (subformat == SF_FORMAT_FLOAT || subformat == SF_FORMAT_DOUBLE)
	{	
		static float float_buffer [BUFFER_LEN] ;
		double	scale ;
		int 	m ;

		sf_command (sndfile, SFC_CALC_SIGNAL_MAX, &scale, sizeof (scale)) ;
			
		if (scale < 1e-10)
			scale = 1.0 ;
		else
			scale = 32700.0 / scale ;

		while ((readcount = sf_read_float (sndfile, float_buffer, BUFFER_LEN)))
		{	
			for (m = 0 ; m < readcount ; m++)
			{				
				buffer [m] = scale * float_buffer [m] ;
			}
									
			write (audio_device, buffer, readcount * sizeof (short));
				
			// 중지하고 싶을때 여기에 조건문을 넣는다.	
			
			if( Sound_Stop_Flag > 0 ) break;		
		}
	}
	else
	{	
		while ((readcount = sf_read_short (sndfile, buffer, BUFFER_LEN)))
		{			
			write (audio_device, buffer, readcount * sizeof (short)) ;
				
			// 중지하고 싶을때 여기에 조건문을 넣는다.		
			if( Sound_Stop_Flag > 0 ) break;		
		}		
	} ;

	close (audio_device) ;
	sf_close (sndfile) ;


	return 0;
	
}; 




//----------------------------------------------------------------------------------------------
//            TITLE   : linux_open_dsp_device
//
//            WORK    : 
//
//            DATE    : 2004. 11. 29
//----------------------------------------------------------------------------------------------
int linux_open_dsp_device (int channels, int srate)
{	int fd, stereo, temp, error ;

	if ((fd = open ("/dev/dsp", O_WRONLY, 0)) == -1 &&
		(fd = open ("/dev/sound/dsp", O_WRONLY, 0)) == -1)
	{	perror ("linux_open_dsp_device : open ") ;
		exit (1) ;
		} ;

	stereo = 0 ;
	if (ioctl (fd, SNDCTL_DSP_STEREO, &stereo) == -1)
	{ 	/* Fatal error */
		perror ("linux_open_dsp_device : stereo ") ;
		exit (1) ;
		} ;

	if (ioctl (fd, SNDCTL_DSP_RESET, 0))
	{	perror ("linux_open_dsp_device : reset ") ;
		exit (1) ;
		} ;

	temp = 16 ;
	if ((error = ioctl (fd, SOUND_PCM_WRITE_BITS, &temp)) != 0)
	{	perror ("linux_open_dsp_device : bitwidth ") ;
		exit (1) ;
		} ;

	if ((error = ioctl (fd, SOUND_PCM_WRITE_CHANNELS, &channels)) != 0)
	{	perror ("linux_open_dsp_device : channels ") ;
		exit (1) ;
		} ;

	if ((error = ioctl (fd, SOUND_PCM_WRITE_RATE, &srate)) != 0)
	{	perror ("linux_open_dsp_device : sample rate ") ;
		exit (1) ;
		} ;

	if ((error = ioctl (fd, SNDCTL_DSP_SYNC, 0)) != 0)
	{	perror ("linux_open_dsp_device : sync ") ;
		exit (1) ;
		} ;

	return 	fd ;
} /* linux_open_dsp_device */

