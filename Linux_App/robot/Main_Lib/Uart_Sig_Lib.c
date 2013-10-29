//----------------------------------------------------------------------------------------------
//            TITLE       : Uart 버퍼 관련 함수 헤더
//            WORK        :
//            DATE        : 2003. 7. 4
//            FILE        : Uart_Q_Lib.h
//            Modified By : Hanchol Cho     - IRSL -
//----------------------------------------------------------------------------------------------



                                                                                                 


                                                                                                 
//----------------------------------------------------------------------------------------------
//            TITLE   : Uart_Sig_Init
//
//            WORK    : 시리얼 시그널 초기화
//
//            DATE    : 2003. 7. 4
//----------------------------------------------------------------------------------------------
void Uart_Sig_Init( int Handle_Ptr )
{
	U8 i;

	//----- 시리얼포트 시그널 핸들러 설정 
	//
	Sig_Action.sa_handler  = Uart_Q_Sig_Handler;
	//Sig_Action.sa_mask     = 0;
	Sig_Action.sa_flags    = 0;
	Sig_Action.sa_restorer = NULL; 
	sigaction( SIGIO, &Sig_Action, NULL );

	// SIGIO 시그널을 받을수 있도록 한다
	fcntl( Handle_Ptr, F_SETOWN, getpid() );  
	fcntl( Handle_Ptr, F_SETFL, FASYNC );  // 파일 디스크립터를 비동기모드로

}	




//----------------------------------------------------------------------------------------------
//            TITLE   : Uart_Sig_Func_Init
//
//            WORK    : 시리얼 시그널 초기화
//
//            DATE    : 2004. 2. 7
//----------------------------------------------------------------------------------------------
void Uart_Sig_Func_Init( int Handle_Ptr, void (*Func_Ptr)(char Data) )
{
	U8 i;

	//----- 시리얼포트 시그널 핸들러 설정 
	//
	Sig_Action.sa_handler  = Uart_Q_Sig_Func_Handler;
	//Sig_Action.sa_mask     = 0;
	Sig_Action.sa_flags    = 0;
	Sig_Action.sa_restorer = NULL; 
	sigaction( SIGIO, &Sig_Action, NULL );

	// SIGIO 시그널을 받을수 있도록 한다
	fcntl( Handle_Ptr, F_SETOWN, getpid() );  
	fcntl( Handle_Ptr, F_SETFL, FASYNC );  // 파일 디스크립터를 비동기모드로

	Sig_Func_Ptr = Func_Ptr;
}	





                                                                                                 
//----------------------------------------------------------------------------------------------
//            TITLE   : Uart_Q_Sig_Handler
//
//            WORK    : 시리얼 포트 핸들러 함수 
//
//            DATE    : 2003. 7. 4
//----------------------------------------------------------------------------------------------
void Uart_Q_Sig_Handler( int Status )
{
	int Receive_Size;
	int i;

	
	Receive_Size = read( Uart_Handle, Uart_Buf, 255 );

	if( Receive_Size > 0 )   // 데이터를 받았다면?
	{
		for( i=0; i<Receive_Size; i++)
		{
			Uart_Q_Push( UART_Q_0, &Uart_Buf[i] );
		}
	}
}





//----------------------------------------------------------------------------------------------
//            TITLE   : Uart_Q_Sig_Func_Handler
//
//            WORK    : 시리얼 포트 핸들러 함수 사용자 정의 함수 실행
//
//            DATE    : 2004. 2. 7
//----------------------------------------------------------------------------------------------
void Uart_Q_Sig_Func_Handler( int Status )
{
	int Receive_Size;
	int i;

	
	Receive_Size = read( Uart_Handle, Uart_Buf, 255 );

	if( Receive_Size > 0 )   // 데이터를 받았다면?
	{
		for( i=0; i<Receive_Size; i++)
		{
			Uart_Q_Push( UART_Q_0, &Uart_Buf[i] );
			
			(*Sig_Func_Ptr)( Uart_Buf[i] );
		}
	}
}


