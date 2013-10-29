//----------------------------------------------------------------------------
//    프로그램명 	: Util 관련 함수
//
//    만든이     	: 
//
//    날  짜     	: 
//    
//    최종 수정  	: 
//
//    MPU_Type		:
//
//    파일명     	: Lb_Util.h
//----------------------------------------------------------------------------



//----- 헤더파일 열기
//
#define  LB_UTIL_LOCAL

#include "Lb_Util.h"




//-- 내부 선언
//



//-- 내부 변수
//
static const char *delim = " \f\n\r\t\v";



//-- 내부 함수
//






void delay(volatile unsigned int timeCount)
{
    while(timeCount --);
}

void delay_second(void)
{
    delay(806596);
}





/*---------------------------------------------------------------------------
     TITLE   : Lb_Util_PaseArgs
     WORK    :
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
int Lb_Util_PaseArgs(char *cmdline, char **argv)
{
	char *tok;
	int argc = 0;

	argv[argc] = NULL;

	for (tok = strtok(cmdline, delim); tok; tok = strtok(NULL, delim))
	{
		argv[argc++] = tok;
	}

	return argc;
}





/*---------------------------------------------------------------------------
     TITLE   : Lb_Util_atoi
     WORK    :
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
int Lb_Util_atoi(char *str)
{
	int i = 0, j = 0;
 
    if (*str == '-' || isdigit(*str)) 
    {
		if(*str == '-') j = 1, str++;

		while(isdigit(*str))
		{
			i = i * 10 + (*(str)) - 48;
			str++;
		}
		if (j == 1) return -i;
		else 		return i;
	}
	else
		return 0;
}