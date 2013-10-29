//-----------------------------------------------------------------------------------------------
//            TITLE       : Key 처리 함수 모음 
//            WORK        :
//            DATE        : 2003. 2. 27
//            FILE        : Key_Lib.c
//            Modified By : Hanchol Cho     - IRSL -
//----------------------------------------------------------------------------------------------




 
struct termios orig_termios;




void KeyBrd_Close( void );









//----------------------------------------------------------------------------------------------
//            TITLE   : init_keyboard
//
//            WORK    : 키처리 초기화
//            
//            DATE    : 2003. 2. 27
//----------------------------------------------------------------------------------------------
void KeyBrd_Init(void)
{
	//
	//------------ 터미널의 속성을 읽어온다
	//
        tcgetattr(0,&initial_settings);
	
	//
	//------------ 터미널은 Canonical Mode, Non Canonical Mode 두가지 있다
	//             여기서는 Non Canonical Mode 를 사용
        new_settings             =  initial_settings;

	//
	//------------ c_lflag <- Local Mode 이다
	//
        new_settings.c_lflag    &= ~ICANON;        // NON Canonical Mode 로 바꾼다
        new_settings.c_lflag    &= ~ECHO;          // ECHO 를 끈다
        new_settings.c_lflag    &= ~ISIG;          // 특정캐릭터에 대해 검사하느것을 하지않음

	//
	//------------ read 실행시 읽어올 문자 갯수... 설정
	//
        new_settings.c_cc[VMIN]  =  1;             // 터미널에서 받을 문자 사이즈 1byte
        new_settings.c_cc[VTIME] =  0;             // Time out = 0 라는것은 Time Out 타이머가 동작 안함
	
        tcsetattr(0, TCSANOW, &new_settings);      // 새로운 값으로 셋팅
}





//----------------------------------------------------------------------------------------------
//            TITLE   : KeyBrd_Close
//
//            WORK    : 터미널 복구
//            
//            DATE    : 2003. 2. 27
//----------------------------------------------------------------------------------------------
void KeyBrd_Close( void )
{
	//
	//------------ 너미널을 원래 상태로 복원한다
	//
        tcsetattr(0,TCSANOW, &initial_settings);
}





//----------------------------------------------------------------------------------------------
//            TITLE   : KeyBrd_Hit
//
//            WORK    : 키가 눌렸는지 검사
//            
//            DATE    : 2003. 2. 27
//----------------------------------------------------------------------------------------------
int KeyBrd_Hit(void)
{
        char ch;
        int nread;

        if(peek_character != -1)
                return 1;

	new_settings.c_cc[VMIN] = 0;           // VMIN = 0 이면 기다리지 않고 바로 1바이트 가져온다
        tcsetattr(0, TCSANOW, &new_settings);
        nread = read(0,&ch,1);                 // 이것이 핵심.. 기다리지 않고 바로 가져옴
	                                       // 데이터가 있냐 없냐에 따라 return 값 다름		
        new_settings.c_cc[VMIN] = 1;           // 다시 원래대로...
        tcsetattr(0,TCSANOW, &new_settings);

	if(nread == 1)
        {
		peek_character = ch;
		return 1;
	}
	
	return 0;
}





//----------------------------------------------------------------------------------------------
//            TITLE   : KeyBrd_Getch_Buf
//
//            WORK    : getch 와 똑같은 구실을 한다
//            
//            DATE    : 2003. 2. 27
//----------------------------------------------------------------------------------------------
int KeyBrd_Getch_Buf( void )
{
        char ch;

	if(peek_character != -1)
        {
		ch = peek_character;
		peek_character = -1;
		return ch;
	}
	
	read(0,&ch,1);

#ifdef  DOS_MODE
	// 엔터키를 0x0d 로 변환한다
	if( ch == 10 ) 	ch = 13;
#endif
	return ch;
}



//----------------------------------------------------------------------------------------------
//            TITLE   : KeyBrd_Getch
//
//            WORK    : 키값을 돌려준다
//            
//            DATE    : 2003. 2. 27
//----------------------------------------------------------------------------------------------
char KeyBrd_Getch(void)
{
	char ch;

	while( !KeyBrd_Hit() );

	ch = KeyBrd_Getch_Buf();

#ifdef  DOS_MODE
	// 엔터키를 0x0d 로 변환한다
	if( ch == 10 ) 	ch = 13;
#endif
	return ch;	
}
                                                                                       
                                                                                                 
                                                                                                 

