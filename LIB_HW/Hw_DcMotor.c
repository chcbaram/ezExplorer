//----------------------------------------------------------------------------
//    프로그램명 	: DC 모터 관련 함수
//
//    만든이     	: Cho Han Cheol
//
//    날  짜     	: 2013. 1. 22.
//    
//    최종 수정  	:
//
//    MPU_Type	:
//
//    파일명     	: Hw_DcMotor.c
//----------------------------------------------------------------------------


//----- 헤더파일 열기
//
#define  HW_DCMOTOR_LOCAL

#include "Hw_DcMotor.h"



#define HW_DCMOTOR_MAX_CNT			10

#define HW_DCMOTOR_STATE_IDLE		0
#define HW_DCMOTOR_STATE_STOP		1
#define HW_DCMOTOR_STATE_RUN		2





void Hw_DcMotor_Tick( void )
{
	u8  i;
	u16 DutyRatio;

	static u16 Cnt = 0;


	for( i=0; i<HW_DCMOTOR_MAX_CH; i++ )
	{
		if( Cnt == 0 )
		{
			if( Hw_DcMotor_State[i].Speed > 0 && Hw_DcMotor_State[i].Dir != HW_DCMOTOR_STOP )
			{
				Hw_DcMotor_Run( i, Hw_DcMotor_State[i].Dir );
				Hw_DcMotor_State[i].Pin = TRUE;
			}
			else
			{
				Hw_DcMotor_Stop(i);
				Hw_DcMotor_State[i].Pin = FALSE;
			}
		}
		else
		{
			//-- 카운트 값을 0~100% 값으로 변환
			//
			DutyRatio = Cnt*HW_DCMOTOR_MAX_PWM/HW_DCMOTOR_MAX_CNT;

			if( DutyRatio > Hw_DcMotor_State[i].Speed && Hw_DcMotor_State[i].Pin == TRUE )
			{
				Hw_DcMotor_Stop(i);
				Hw_DcMotor_State[i].Pin = FALSE;
			}
		}
	}


	Cnt++;

	if( Cnt >= HW_DCMOTOR_MAX_CNT ) Cnt = 0;
}





/*---------------------------------------------------------------------------
     TITLE   : Hw_DcMotor_Init
     WORK    : 
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
void Hw_DcMotor_Init( void )
{
	u8 i;


	//-- GPIO 설정
	//
	//-- L0 PB.0
	//
	REG_GPIOB_CRL &= ~(0x0F << ( 0));	// Clear
	REG_GPIOB_CRL |=  (0x03 << ( 0));	// MODE, PB.0 Output mode, max speed 50Mhz
	REG_GPIOB_CRL |=  (0x00 << ( 2));	// CNF,  PB.0 General purpose output push-pul	

	//-- L1 PB.1
	//
	REG_GPIOB_CRL &= ~(0x0F << ( 4));	// Clear
	REG_GPIOB_CRL |=  (0x03 << ( 4));	// MODE, PB.1 Output mode, max speed 50Mhz
	REG_GPIOB_CRL |=  (0x00 << ( 6));	// CNF,  PB.1 General purpose output push-pul	

	//-- L2 PB.2
	//
	REG_GPIOB_CRL &= ~(0x0F << ( 8));	// Clear
	REG_GPIOB_CRL |=  (0x03 << ( 8));	// MODE, PB.2 Output mode, max speed 50Mhz
	REG_GPIOB_CRL |=  (0x00 << (10));	// CNF,  PB.2 General purpose output push-pul	

	//-- L3 PB.10
	//
	REG_GPIOB_CRH &= ~(0x0F << ( 8));	// Clear
	REG_GPIOB_CRH |=  (0x03 << ( 8));	// MODE, PB.2 Output mode, max speed 50Mhz
	REG_GPIOB_CRH |=  (0x00 << (10));	// CNF,  PB.2 General purpose output push-pul	

	//-- R0 PB.11
	//
	REG_GPIOB_CRH &= ~(0x0F << (12));	// Clear
	REG_GPIOB_CRH |=  (0x03 << (12));	// MODE, PB.2 Output mode, max speed 50Mhz
	REG_GPIOB_CRH |=  (0x00 << (14));	// CNF,  PB.2 General purpose output push-pul	
	
	//-- R1 PB.12
	//
	REG_GPIOB_CRH &= ~(0x0F << (16));	// Clear
	REG_GPIOB_CRH |=  (0x03 << (16));	// MODE, PB.6 Output mode, max speed 50Mhz
	REG_GPIOB_CRH |=  (0x00 << (18));	// CNF,  PB.6 General purpose output push-pul	
	
	//-- R2 PB.13
	//
	REG_GPIOB_CRH &= ~(0x0F << (20));	// Clear
	REG_GPIOB_CRH |=  (0x03 << (20));	// MODE, PB.6 Output mode, max speed 50Mhz
	REG_GPIOB_CRH |=  (0x00 << (22));	// CNF,  PB.6 General purpose output push-pul	

	//-- R3 PB.14
	//
	REG_GPIOB_CRH &= ~(0x0F << (24));	// Clear
	REG_GPIOB_CRH |=  (0x03 << (24));	// MODE, PB.7 Output mode, max speed 50Mhz
	REG_GPIOB_CRH |=  (0x00 << (26));	// CNF,  PB.7 General purpose output push-pul				




	for( i=0; i<HW_DCMOTOR_MAX_CH; i++ )
	{
		Hw_DcMotor_Stop( i );

		Hw_DcMotor_State[i].Dir   = HW_DCMOTOR_STOP;
		Hw_DcMotor_State[i].Pin   = FALSE;
		Hw_DcMotor_State[i].Speed = 0;
		Hw_DcMotor_State[i].State = HW_DCMOTOR_STATE_STOP;
	}

	Hw_Timer_Set  ( HW_TIMER_CH_DCMOTOR, 1, LOOP_TIME, Hw_DcMotor_Tick, NULL );
	Hw_Timer_Start( HW_TIMER_CH_DCMOTOR );
}





/*---------------------------------------------------------------------------
     TITLE   : Hw_DcMotor_Run
     WORK    :
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
void Hw_DcMotor_Run( u8 Ch, u8 Dir )
{
	switch( Ch )
	{
		case 0:
			if( Dir == HW_DCMOTOR_FOR )
			{
				CLR_BIT( REG_GPIOB_ODR, HW_DCMOTOR_PORT_0_L );
				SET_BIT( REG_GPIOB_ODR, HW_DCMOTOR_PORT_0_R );
			}
			else
			{
				SET_BIT( REG_GPIOB_ODR, HW_DCMOTOR_PORT_0_L );
				CLR_BIT( REG_GPIOB_ODR, HW_DCMOTOR_PORT_0_R );
			}
			break;

		case 1:
			if( Dir == HW_DCMOTOR_FOR )
			{
				CLR_BIT( REG_GPIOB_ODR, HW_DCMOTOR_PORT_1_L );
				SET_BIT( REG_GPIOB_ODR, HW_DCMOTOR_PORT_1_R );
			}
			else
			{
				SET_BIT( REG_GPIOB_ODR, HW_DCMOTOR_PORT_1_L );
				CLR_BIT( REG_GPIOB_ODR, HW_DCMOTOR_PORT_1_R );
			}
			break;

		case 2:
			if( Dir == HW_DCMOTOR_FOR )
			{
				SET_BIT( REG_GPIOB_ODR, HW_DCMOTOR_PORT_2_L );
				CLR_BIT( REG_GPIOB_ODR, HW_DCMOTOR_PORT_2_R );
			}
			else
			{
				CLR_BIT( REG_GPIOB_ODR, HW_DCMOTOR_PORT_2_L );
				SET_BIT( REG_GPIOB_ODR, HW_DCMOTOR_PORT_2_R );
			}
			break;

		case 3:
			if( Dir == HW_DCMOTOR_FOR )
			{
				CLR_BIT( REG_GPIOB_ODR, HW_DCMOTOR_PORT_3_L );
				SET_BIT( REG_GPIOB_ODR, HW_DCMOTOR_PORT_3_R );
			}
			else
			{
				SET_BIT( REG_GPIOB_ODR, HW_DCMOTOR_PORT_3_L );
				CLR_BIT( REG_GPIOB_ODR, HW_DCMOTOR_PORT_3_R );
			}
			break;
	}
}





/*---------------------------------------------------------------------------
     TITLE   : Hw_DcMotor_Stop
     WORK    :
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
void Hw_DcMotor_Stop( u8 Ch )
{
	switch( Ch )
	{
		case 0:
			CLR_BIT( REG_GPIOB_ODR, HW_DCMOTOR_PORT_0_L );
			CLR_BIT( REG_GPIOB_ODR, HW_DCMOTOR_PORT_0_R );
			break;

		case 1:
			CLR_BIT( REG_GPIOB_ODR, HW_DCMOTOR_PORT_1_L );
			CLR_BIT( REG_GPIOB_ODR, HW_DCMOTOR_PORT_1_R );
			break;

		case 2:
			CLR_BIT( REG_GPIOB_ODR, HW_DCMOTOR_PORT_2_L );
			CLR_BIT( REG_GPIOB_ODR, HW_DCMOTOR_PORT_2_R );
			break;

		case 3:
			CLR_BIT( REG_GPIOB_ODR, HW_DCMOTOR_PORT_3_L );
			CLR_BIT( REG_GPIOB_ODR, HW_DCMOTOR_PORT_3_R );
			break;
	}
}





/*---------------------------------------------------------------------------
     TITLE   : Hw_DcMotor_SetPwm
     WORK    :
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
void Hw_DcMotor_SetPwm( u8 Ch, u16 Pwm  )
{
	if( Ch >= HW_DCMOTOR_MAX_CH ) return;

	if( Pwm > HW_DCMOTOR_MAX_PWM ) Pwm = HW_DCMOTOR_MAX_PWM;

	Hw_DcMotor_State[Ch].Speed = Pwm;
}



/*---------------------------------------------------------------------------
     TITLE   : Hw_DcMotor_SetDir
     WORK    :
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
void Hw_DcMotor_SetDir( u8 Ch, u8 Dir  )
{
	if( Ch >= HW_DCMOTOR_MAX_CH ) return;

	Hw_DcMotor_State[Ch].Dir = Dir;
}





/*---------------------------------------------------------------------------
     TITLE   : Hw_DcMotor_Handle
     WORK    :
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
void Hw_DcMotor_Handle( s16 SpeedLeft, s16 SpeedRight )
{
	u16 Pwm;


	if( SpeedLeft  >  HW_DCMOTOR_MAX_PWM )	SpeedLeft  =  HW_DCMOTOR_MAX_PWM;
	if( SpeedLeft  < -HW_DCMOTOR_MAX_PWM )	SpeedLeft  = -HW_DCMOTOR_MAX_PWM;
	if( SpeedRight >  HW_DCMOTOR_MAX_PWM )	SpeedRight =  HW_DCMOTOR_MAX_PWM;
	if( SpeedRight < -HW_DCMOTOR_MAX_PWM )	SpeedRight = -HW_DCMOTOR_MAX_PWM;


	if( SpeedLeft > 0 )
	{
		Pwm = SpeedLeft;
		Hw_DcMotor_SetPwm( HW_DCMOTOR_LU, Pwm );
		Hw_DcMotor_SetPwm( HW_DCMOTOR_LD, Pwm );		
		Hw_DcMotor_SetDir( HW_DCMOTOR_LU, HW_DCMOTOR_FOR );
		Hw_DcMotor_SetDir( HW_DCMOTOR_LD, HW_DCMOTOR_FOR );		
	}
	else
	if( SpeedLeft < 0 )
	{
		Pwm = -SpeedLeft;
		Hw_DcMotor_SetPwm( HW_DCMOTOR_LU, Pwm );
		Hw_DcMotor_SetPwm( HW_DCMOTOR_LD, Pwm );		
		Hw_DcMotor_SetDir( HW_DCMOTOR_LU, HW_DCMOTOR_BACK );
		Hw_DcMotor_SetDir( HW_DCMOTOR_LD, HW_DCMOTOR_BACK );		
	}
	else
	{
		Hw_DcMotor_SetDir( HW_DCMOTOR_LU, HW_DCMOTOR_STOP );
		Hw_DcMotor_SetDir( HW_DCMOTOR_LD, HW_DCMOTOR_STOP );				
	}


	if( SpeedRight > 0 )
	{
		Pwm = SpeedRight;
		Hw_DcMotor_SetPwm( HW_DCMOTOR_RU, Pwm );
		Hw_DcMotor_SetPwm( HW_DCMOTOR_RD, Pwm );		
		Hw_DcMotor_SetDir( HW_DCMOTOR_RU, HW_DCMOTOR_FOR );
		Hw_DcMotor_SetDir( HW_DCMOTOR_RD, HW_DCMOTOR_FOR );		
	}
	else
	if( SpeedRight < 0 )
	{
		Pwm = -SpeedRight;
		Hw_DcMotor_SetPwm( HW_DCMOTOR_RU, Pwm );
		Hw_DcMotor_SetPwm( HW_DCMOTOR_RD, Pwm );		
		Hw_DcMotor_SetDir( HW_DCMOTOR_RU, HW_DCMOTOR_BACK );
		Hw_DcMotor_SetDir( HW_DCMOTOR_RD, HW_DCMOTOR_BACK );		
	}
	else
	{
		Hw_DcMotor_SetDir( HW_DCMOTOR_RU, HW_DCMOTOR_STOP );
		Hw_DcMotor_SetDir( HW_DCMOTOR_RD, HW_DCMOTOR_STOP );				
	}

}
