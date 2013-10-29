//----------------------------------------------------------------------------
//    프로그램명 	: StrCmd_MOVE 관련 함수
//
//    만든이     	: 
//
//    날  짜     	: 
//    
//    최종 수정  	: 
//
//    MPU_Type 		:
//
//    파일명     	: AP_StrCmd_MOVE.c
//----------------------------------------------------------------------------





//----- 헤더파일 열기
//
#define  AP_STRCMD_MOVE_LOCAL  

#include "Ap_StrCmd.h"


#include <stdio.h>
//#include <ctype.h>
#include <string.h>


//-- 내부 선언
//



//-- 내부 변수
//




/*---------------------------------------------------------------------------
     TITLE   : Ap_StrCmd_MOVE
     WORK    : 
     ARG     : void
     RET     : void   
---------------------------------------------------------------------------*/
void Ap_StrCmd_MOVE( u8 OpCode,  void *arg )
{
	char CmdMain[10];
	char CmdSub[10];
	int  LeftSpeed;
	int  RightSpeed;

	int  Cnt = -1;

	u8	 ErrCode = ERR_NONE;

	char *argv[128];
	u16  argc;

	//sscanf((char *)arg, "%s %s %d %d", CmdMain, CmdSub, &LeftSpeed, &RightSpeed);

	argc = Lb_Util_PaseArgs( (char *)arg, argv );


	if( strcmp( argv[1], "PWM" ) == 0 )
	{
		
		LeftSpeed  = Lb_Util_atoi(argv[2]);
		RightSpeed = Lb_Util_atoi(argv[3]); 


		Hw_DcMotor_Handle( LeftSpeed, RightSpeed );

		Ap_StrCmd_SendResp( ErrCode, "%d %d\n", LeftSpeed, RightSpeed );
	}
	else
	{
		ErrCode = 0xFF;
	}

	Ap_StrCmd_SendResp( ErrCode, "\n" );
}
