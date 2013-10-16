//----------------------------------------------------------------------------
//    프로그램명 	: StrCmd_List 관련 함수
//
//    만든이     	: 
//
//    날  짜     	: 
//    
//    최종 수정  	: 
//
//    MPU_Type 		:
//
//    파일명     	: AP_StrCmd_List.c
//----------------------------------------------------------------------------





//----- 헤더파일 열기
//
#define  AP_STRCMD_LIST_LOCAL  

#include "Ap_StrCmd_List.h"


#include <ctype.h>
#include <string.h>


//-- 내부 선언
//



//-- 내부 변수
//


/*---------------------------------------------------------------------------
     TITLE   : Ap_StrCmd_List
     WORK    : 
     ARG     : void
     RET     : void   
---------------------------------------------------------------------------*/
void Ap_StrCmd_List( void )
{
	Ap_StrCmd_AddCmd( "MOVE", "Robot Move",		  Ap_StrCmd_MOVE );
}


