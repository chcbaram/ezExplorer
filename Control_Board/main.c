//----------------------------------------------------------------------------
//    프로그램명 	: ezExplorer
//
//    만든이     	: Cho Han Cheol (Baram)
//
//    날  짜     	: 2013.10.14.
//
//    최종 수정  	:
//
//    MPU_Type		:
//
//    파일명     	: Main.c
//----------------------------------------------------------------------------
/*
	131014 
		- 초기 프로젝트 작업 시작		
*/




//----- 헤더파일 열기
//
#define  MAIN_LOCAL

#include "Main.h"




//-- 내부 선언
//
#define DCM_RU	3
#define DCM_RD	1
#define DCM_LU	2
#define DCM_LD	0


//-- 내부 변수
//



//-- 내부 함수
//
static void Main_Init( void );

void Test_DCMOTOR();



void LED_Tick( void )
{
	Hw_Led_Toggle(0);
}



/*---------------------------------------------------------------------------
     TITLE   :	main          
     WORK    : 	
     			메인 함수
     ARG     : 	void
     RET     : 	
     			int
---------------------------------------------------------------------------*/
int main(void)
{
	
	Main_Init();

	//-- 명령어 처리
	//
	//Ap_RadioMenu_ExeCmd();    
	//Ap_GLcdMenu_ExeCmd();    
	//Ap_EduMenu_ExeCmd();

	Lb_printf("ezExplorer\n");
    while(1)
    {
    	//Test_DCMOTOR();
    	Ap_StrCmd_ExeCmd();
    }
    


    return 0;
}





/*---------------------------------------------------------------------------
     TITLE   : Main_Init
     WORK    :
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
void Main_Init( void )
{
	Hw_Init();
	Ap_Init();
	
	Hw_Timer_Set  ( HW_TIMER_CH_LED, 1000, LOOP_TIME, LED_Tick, NULL );
	Hw_Timer_Start( HW_TIMER_CH_LED );	
}





void Test_DCMOTOR()
{
	u8  Ch;
	static s16 Pwm = 0;

	Ch = Hw_Uart_Getch( 0 );

	if( Ch == 'i' )
	{
		Hw_DcMotor_Handle( Pwm, Pwm );
	}
	else if( Ch == ',' )
	{
		Hw_DcMotor_Handle( -Pwm, -Pwm );
	}
	else if( Ch == 'j' )
	{
		Hw_DcMotor_Handle( -Pwm, Pwm );
	}
	else if( Ch == 'l' )
	{
		Hw_DcMotor_Handle( Pwm, -Pwm );
	}
	else if( Ch == 'k' )
	{
		Hw_DcMotor_Handle( 0, 0 );
	}
	else if( Ch == 'q' )
	{
		Pwm += 10;

		if( Pwm > 100 ) Pwm = 100;

	}
	else if( Ch == 'a' )
	{
		if( Pwm >= 10 ) Pwm -= 10;
	}

	Hw_Uart_Print( 0, "Pwm %d\n", Pwm );
}



