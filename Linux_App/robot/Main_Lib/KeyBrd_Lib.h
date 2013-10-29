//-----------------------------------------------------------------------------------------------
//            TITLE       : KeyBoard 처리 함수 헤더`
//            WORK        :
//            DATE        : 2003. 7. 3
//            FILE        : KeyBrd_Lib.h
//            Modified By : Hanchol Cho     - IRSL -
//----------------------------------------------------------------------------------------------


#ifndef  _KEYBRD_LIB_H_
#define  _KEYBRD_LIB_H_    

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include <time.h>                                                                                             

                                                                                                 
                                                                                                 
//-----------------------------------------------------------------------------------------------//            TITLE   : 변수 선언
//            WORK    :
//            DATE    : 2003. 2. 27
//----------------------------------------------------------------------------------------------
                                                                                                 
static struct termios            initial_settings;
static struct termios            new_settings;
static int    peek_character =  -1;


void KeyBrd_Init( void );
void KeyBrd_Close( void );
int KeyBrd_Hit( void );
int KerBrd_Getch_Buf( void );
char KeyBrd_Getch( void );



#endif                                                                                                 

