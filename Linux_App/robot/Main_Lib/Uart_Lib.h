//----------------------------------------------------------------------------------------------
//            TITLE       : Serial 통신 처리 함수 모음
//            WORK        :
//            DATE        : 2003. 3. 7
//            FILE        : Uart_Lib.h
//            Modified By : Hanchol Cho     - IRSL -
//----------------------------------------------------------------------------------------------
                                                                                                 
                                                                                                 
#ifndef  _UART_LIB_H
#define  _UART_LIB_H
                                                                                                 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>                                                                                                 
                   
#define  BAUD_115200  6                                                                          
#define  BAUD_57600   5
#define  BAUD_38400   4
#define  BAUD_19200   3
#define  BAUD_9600    2
#define  BAUD_2400    1
#define  BAUD_1200    0                                                                                                 
                                                                                                 
#define _POSIX_SOURCE     1


                                                                                                 
#define  FALSE  0
#define  TRUE   1
                                                                                                 
#define  COM1   0
#define  COM2   1
#define  COM3   2
#define  COM4   3

#define  COME_EX1 4
#define  COME_EX2 5

#define  COM_USB0	6
#define  COM_USB1	7

                                                                                                 
#define  CANON_MODE       0
#define  NONE_CANON_MODE  1
                                                                                                 
                                                                                                 
volatile  int  STOP = FALSE;
                                                                                                 
                                                                                                 
int Uart_Handle;   // 시리얼 포트 핸들

                                                                                                 
struct termios     Term_OldIo;
struct termios     Term_NewIo;
                                                                                                 
char   Uart_Buf[255], Buf_ReadSize;



                                                                                                 
char   *Uart_PortStr[11] = { "/dev/ttyS0",      // 시리얼 포트 설정 배열
                           "/dev/ttyS1",
                           "/dev/ttyS2",
                           "/dev/ttyS3",
                           "/dev/ttyE0",
                           "/dev/ttyE1",
                           "/dev/ttyUSB0",
                           "/dev/ttyACM0" };
                                                                                                 
int    Uart_Baud[7]  = { B1200, B2400, B9600, B19200, B38400, B57600, B115200 };  // 통신 속도 설정 배열
                                                                                                 
int    Uart_ModeFlag;   // 표준 모드인지, 비표준인지 설정                              


//----- 함수 재정의 
//                                                                   
#define Uart_Open(PortNum,BaudData)    Uart_OpenPortNonCanonical_Mode(PortNum,BaudData)                                                                                                                                                                                         

void Uart_Close( void );


#endif
                                                                                                 
                                                                                                 

