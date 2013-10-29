

#ifndef _MAIN_H_
#define _MAIN_H_

//-- For OpenNI2
//
#include <stdio.h>
#include <OpenNI.h>

//-- For OpenCV
//
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include <time.h>



#include "./Main_Lib/Define.h"


#define IMG_WIDTH			320		
#define IMG_HEIGHT			240		

#define DEPTH_IMG_WIDTH		320		
#define DEPTH_IMG_HEIGHT	240		
#define DEPTH_IMG_FPS		20

#define COLOR_IMG_WIDTH		320		
#define COLOR_IMG_HEIGHT	240		
#define COLOR_IMG_FPS		20


extern void Sleep(int millisecs);

#endif