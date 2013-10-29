//----------------------------------------------------------------------------
//    프로그램명 	: StrCmd MOVE 관련 함수 헤더
//
//    만든이     	: 
//
//    날  짜     	: 
//    
//    최종 수정  	: 
//
//    MPU_Type 		:
//
//    파일명     	: AP_StrCmd_MOVE.h
//----------------------------------------------------------------------------
                                                                                                 
                                                                                                 
#ifndef  _AP_STRCMD_MOVE_H
#define  _AP_STRCMD_MOVE_H


#ifdef   AP_STRCMD_MOVE_LOCAL
#define  EXT_AP_STRCMD_MOVE_DEF 
#else
#define  EXT_AP_STRCMD_MOVE_DEF     extern
#endif


#include "Ap.h"



EXT_AP_STRCMD_MOVE_DEF void Ap_StrCmd_MOVE( u8 OpCode,  void *arg );


#endif
