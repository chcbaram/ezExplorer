//----------------------------------------------------------------------------------------------
//            TITLE       : Uart 버퍼 관련 함수 헤더
//            WORK        :
//            DATE        : 2003. 7. 4
//            FILE        : Uart_Sig_Lib.h
//            Modified By : Hanchol Cho     - IRSL -
//----------------------------------------------------------------------------------------------
                                                                                                 
                                                                                                 
#ifndef  _UART_SIG_LIB_H
#define  _UART_SIG_LIB_H


#include "Uart_Q_Lib.c"



//----------------- 시그널 관련 
//

#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>



struct sigaction Sig_Action;    // 시그널 액션 정의

// 시리얼 데이터가 왔을때 실행하고자 하는 함수 포인터
//
void ( *Sig_Func_Ptr )( char Data );


void Uart_Sig_Init( int Handle_Ptr );
void Uart_Sig_Func_Init( int Handle_Ptr, void (*Func_Ptr)(char Data) );
void Uart_Sig_Func_Init( int Handle_Ptr, void (*Func)(char Data) );
void Uart_Q_Sig_Func_Handler( int Status );



void Uart_Q_Sig_Handler( int Status );






#endif

