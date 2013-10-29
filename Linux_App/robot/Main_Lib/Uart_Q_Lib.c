//----------------------------------------------------------------------------
//    프로그램명 : Uart 버퍼 관련 
//
//    만든이     : Hanchol Cho
//
//    날  짜     : 2003.7.4
//
//    MPU        : 
//    파일명     : Uart_Q_Lib.c
//----------------------------------------------------------------------------


#define UART_Q_0    0



#define UART_BUFFER_OBJ_MAX 2

#define UART_BUFFER_MAX     50                //  버퍼 최대 갯수
#define UART_DATA_PTR_TYPE  char              //  데이터 포멧 형식 



#define UART_Q_EMPTY        1
#define UART_Q_FULL         0
#define UART_POP_SUCCESS    2

#define UART_Q_SIZE(x)       Uart_Q_Size[x]       // 버퍼의 사이즈를 돌려준다.
#define UART_Q_VAILD(x)      Uart_Q_Size[x]       // 캔에 데이터가 있는지 검사      

             
UART_DATA_PTR_TYPE    Uart_Q_Buffer[ UART_BUFFER_OBJ_MAX ][ UART_BUFFER_MAX+1 ];  // 들어오는 데이터 버퍼


S16  Uart_Q_Size[ UART_BUFFER_OBJ_MAX ];                       // 버퍼의 데이터 숫자  
U16  Uart_Q_Start[ UART_BUFFER_OBJ_MAX ];                      // 버퍼의 시작 포인터
U16  Uart_Q_End[ UART_BUFFER_OBJ_MAX ];                        // 버퍼의 끝 포인터





//----------------------------------------------------------------------------
//
//          TITLE : Uart_Q_Init
//
//          WORK  : 캔의 Q 버퍼를 초기화 한다
//
//----------------------------------------------------------------------------
void Uart_Q_Init(void)
{
	U8 i;
	
	for( i=0; i < UART_BUFFER_OBJ_MAX; i++)
	{
        	Uart_Q_Size[i]  = 0;
        	Uart_Q_Start[i] = 0;    // 큐의 시작점 초기화
        	Uart_Q_End[i]   = 0;    // 큐의 끝점 초기화
      	}
}        





//----------------------------------------------------------------------------
//
//          TITLE : Uart_Q_Add
//
//          WORK  : 캔의 Q 버퍼에 데이터 삽입
//
//                  0 : 에러 
//----------------------------------------------------------------------------
S8 Uart_Q_Push(U8 Buf_Index, UART_DATA_PTR_TYPE  *PushData )
{
        
        // 데이터의 끝 포인터는 항상 비어있는 공간을 가르키고 있다.
        //  |      |
        //  |      | <- End 포인트
        //  | Start |
        // 데이터를 End 포인터에 먼저쓰고 포인터를 증가시킨다.


	// 만약을 위해 사이즈를 초기화
	//
        if( Uart_Q_End[Buf_Index] == Uart_Q_Start[Buf_Index]) Uart_Q_Size[Buf_Index] = 0;
                
                
	// 큐 버퍼에 데이터 삽입
	//                
        Uart_Q_Buffer[Buf_Index][ Uart_Q_End[Buf_Index] ] = *PushData;
        
        
        
        if( ++Uart_Q_End[Buf_Index] >= UART_BUFFER_MAX )    // 키 버퍼의 끝자리에 오면
        {                                      // 포인터를 처음으로 돌린다
                if( Uart_Q_Start[Buf_Index] )        // 시작포인터가 처음에 있지 않으면
                {
                        Uart_Q_End[Buf_Index] = 0;   // 끝포인터를 처음으로 옮긴다.
                }                        
                else                     // 시작포인터가 처음에 있으면
                {                        // 시작포인터를 다음으로 옮겨야 한다.
                        Uart_Q_Start[Buf_Index]++;  
                        Uart_Q_End[Buf_Index]  = 0;
                }                                                
        }       
        else if( Uart_Q_End[Buf_Index] == Uart_Q_Start[Buf_Index] )      // 끝포인터랑 시작포인터랑 같으면
        {                                        // 시작포인터를 다음으로 홈겨야한다.
                if( ++Uart_Q_Start[Buf_Index] >= UART_BUFFER_MAX )
                {
                        Uart_Q_Start[Buf_Index] = 0;          // 옮길려고하는 시작포인터가
                                                                // 끝에있으면 처음으로 옮긴다.
                }                        
        }
        
        // 뷰 사이즈 값이 넘치지 않도록 하자
        //
        if( ++Uart_Q_Size[Buf_Index] > UART_BUFFER_MAX)
        { 
		Uart_Q_Size[Buf_Index] = UART_BUFFER_MAX;
	}

	return 0; 
}                                                  





//----------------------------------------------------------------------------
//
//          TITLE : Uart_Q_Pop
//
//          WORK  : 캔의 Q 버퍼에 데이터 가져온다.
//
//                  
//----------------------------------------------------------------------------

S8 Uart_Q_Pop(U8 Buf_Index, UART_DATA_PTR_TYPE *Motion_Data_Ptr )
{
        U16 Pop_Index;

	Pop_Index = Uart_Q_Start[Buf_Index];    // 꺼내올 데이터 인덱스 저장
	
        if( Uart_Q_End[Buf_Index] == Uart_Q_Start[Buf_Index])
        {
        	Uart_Q_Size[Buf_Index] = 0;
        	return UART_Q_EMPTY; // 버퍼가 비어있으면
	}	
	
	// 큐의 끝에 걸렸다면 다시 첨으로 돌린다.
	//                                
        if( ++Uart_Q_Start[Buf_Index] >= UART_BUFFER_MAX)
                Uart_Q_Start[Buf_Index] = 0;

	
	// 사이즈는 한개의 데이터를 꺼냈으므로 줄인다
	//
        Uart_Q_Size[Buf_Index]--;
        
        // 언더 플로워 방지
        //
        if( Uart_Q_Size[Buf_Index] < 0 ) Uart_Q_Size[Buf_Index] = 0;
        
        // 큐 버퍼의 내용을 저장코져하는 원하는 전달인수에 카피한다
        //
        *Motion_Data_Ptr =  Uart_Q_Buffer[Buf_Index][ Pop_Index ];
        
        
        
        return UART_POP_SUCCESS;     // 로드 성공
}



