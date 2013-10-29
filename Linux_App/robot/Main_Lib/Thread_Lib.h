//----------------------------------------------------------------------------------------------
//            TITLE       : Thread_Lib
//            WORK        :
//            DATE        : 2004. 11. 29
//            FILE        : Thread_Lib.h
//            Modified By : Hanchol Cho     - IRSL -
//----------------------------------------------------------------------------------------------
                                                                                                 
                                                                                                 
#ifndef  _THREAD_LIB_H
#define  _THREAD_LIB_H


#ifdef   THREAD_LIB_LOCAL
#define  EXT_THREAD_LIB_DEF 
#else
#define  EXT_THREAD_LIB_DEF     extern
#endif

#ifdef   LIB_INCLUDE
#undef  EXT_THREAD_LIB_DEF
#define EXT_THREAD_LIB_DEF
#endif

            

#include "Define.h"
#include <pthread.h>


#define   THREAD_FUNC(x) 	*x(void *Arg)


#define   THREAD_MAX     10   // 최대 스레드 갯수
#define   THREAD_USE     1
#define   THREAD_NOT_USE 0

#define   DELAY_MS(x)    usleep(x*1000)

typedef struct  // 스레드 구조체
{

	pthread_t   Thread_ID;	
	int         Thread_Num;
	int         Thread_Use;
	int         Thread_Time;
	int         Thread_Mode;
	int			Thread_Stop;
	int 		Thread_Ret;
	void 		*Thread_Arg;
	void		*Thread_Arg_Etc;
	void        *(*Thread_Func)(void *);
} THREAD_OBJ;





EXT_THREAD_LIB_DEF THREAD_OBJ   Thread_Obj[ THREAD_MAX ];  // 스레드 선언


EXT_THREAD_LIB_DEF void Thread_Create( U16 ID, void *(*Thread_Func)(void *), void *Arg_Etc  );
EXT_THREAD_LIB_DEF void Thread_Start( U16 ID );
EXT_THREAD_LIB_DEF void Thread_Stop( U16 ID );
EXT_THREAD_LIB_DEF void Thread_Stop_All( void );

#endif

