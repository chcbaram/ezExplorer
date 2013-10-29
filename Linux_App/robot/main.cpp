
//-- For OpenNI2
//
#include <stdio.h>
#include <OpenNI.h>

#include "OniSampleUtilities.h"


//-- For OpenCV
//
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include <time.h>
#include <stdlib.h>
#include <fcntl.h>

#define THREAD_LIB_LOCAL

#include "./Main_Lib/KeyBrd_Lib.h"
#include "./Main_Lib/Thread_Lib.h"                                                                                                 
                                                                                                
#include "./Main_Lib/KeyBrd_Lib.c"
#include "./Main_Lib/Thread_Lib.c"



extern void *Lib_Motor(void *Arg);
extern void *Lib_Vision(void *Arg);

int Main_Init();
int wasKeyboardHit(int *pCh);


extern int Lib_Motor_Debug;
extern int Lib_Vision_Debug;




int main()
{
	int Ch;

	Main_Init();

	printf("Robot Ver 1.0\n");

	Thread_Create( 0, Lib_Motor, NULL  );
	Thread_Create( 1, Lib_Vision, NULL  );


	U8 Key_Data;


	KeyBrd_Init();

	Thread_Start(0);
	Thread_Start(1);

	while(1)
	{
		//if( wasKeyboardHit( &Ch ) )
		if( KeyBrd_Hit() )
		{
			Key_Data = KeyBrd_Getch();

			if( Key_Data == 'q' ) break;


			if( Key_Data == '1' )	Lib_Motor_Debug  ^= 1;
			if( Key_Data == '2' )	Lib_Vision_Debug ^= 1;
			


			
			if( Key_Data == 'w' )
			{
				
			}

			printf("KeyPressed\n");
		}		
	}

	Thread_Stop_All();

	KeyBrd_Close();

	printf("Robot End...\n");
	return 0;
}




int Main_Init(void)
{
	return true;
}




void Sleep(int millisecs)
{
	usleep(millisecs * 1000);
}

