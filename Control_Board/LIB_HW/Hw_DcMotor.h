//----------------------------------------------------------------------------
//    프로그램명 	:
//
//    만든이     	: Cho Han Cheol
//
//    날  짜     	:
//    
//    최종 수정  	:
//
//    MPU_Type	:
//
//    파일명     	: Hw_Led.h
//----------------------------------------------------------------------------
                                                                                                 
#ifndef __HW_DCMOTOR_H__
#define __HW_DCMOTOR_H__


#ifdef   HW_DCMOTOR_LOCAL
#define  EXT_HW_DCMOTOR_DEF
#else
#define  EXT_HW_DCMOTOR_DEF     extern
#endif



#include "Hw.h"


#define HW_DCMOTOR_MAX_CH	4

#ifndef ON
#define ON  1
#endif

#ifndef OFF
#define OFF 0
#endif


#define HW_DCMOTOR_MAX_PWM			100


#define HW_DCMOTOR_FOR			0
#define HW_DCMOTOR_BACK			1
#define HW_DCMOTOR_STOP			2


#define HW_DCMOTOR_PORT_0_L		0
#define HW_DCMOTOR_PORT_0_R		1
#define HW_DCMOTOR_PORT_1_L		2
#define HW_DCMOTOR_PORT_1_R		10
#define HW_DCMOTOR_PORT_2_L		11
#define HW_DCMOTOR_PORT_2_R		12
#define HW_DCMOTOR_PORT_3_L		13
#define HW_DCMOTOR_PORT_3_R		14



#define HW_DCMOTOR_RU			3
#define HW_DCMOTOR_RD			1
#define HW_DCMOTOR_LU			2
#define HW_DCMOTOR_LD			0




typedef struct
{
	u8  State;
	u8  Dir;
	u8  Pin;
	u16 Speed;
} HW_DCMOTOR_OBJ;


EXT_HW_DCMOTOR_DEF HW_DCMOTOR_OBJ	Hw_DcMotor_State[HW_DCMOTOR_MAX_CH];


EXT_HW_DCMOTOR_DEF void Hw_DcMotor_Init( void );
EXT_HW_DCMOTOR_DEF void Hw_DcMotor_Run( u8 Ch, u8 Dir );
EXT_HW_DCMOTOR_DEF void Hw_DcMotor_Stop( u8 Ch );

EXT_HW_DCMOTOR_DEF void Hw_DcMotor_SetPwm( u8 Ch, u16 Pwm  );
EXT_HW_DCMOTOR_DEF void Hw_DcMotor_SetDir( u8 Ch, u8 Dir  );

EXT_HW_DCMOTOR_DEF void Hw_DcMotor_Handle( s16 SpeedLeft, s16 SpeedRight );



#endif
