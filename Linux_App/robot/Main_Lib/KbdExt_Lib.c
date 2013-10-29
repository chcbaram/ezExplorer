


//----------------------------------------------------------------------------------------------
//            TITLE   : KbdExt_CheckMeta
//
//            WORK    : 
//            
//            DATE    : 2004. 3. 19
//----------------------------------------------------------------------------------------------
S16	KbdExt_CheckMeta( const char *Str_Ptr )
{
	U8   i        = 0;
	S16  Ret_Flag = 0;

	for( i = 0; i < KBD_EXT_MAX_META_STR; i++ ) 
	{
		// 메타 문자열 배열에서 데이터 비교후 Ret_Flag 에 
		// 메타 문자열에 해당하는 비트 위치에 설정함
		//
		if ( strcasecmp( Str_Ptr, KbdExt_MetaStr_Left[ i ] ) == 0 ) 
		{
			Ret_Flag = 1 << i;
		}

		if ( strcasecmp( Str_Ptr, KbdExt_MetaStr_Right[ i ] ) == 0 ) 
		{
			Ret_Flag = 1 << i;
		}
		
	}

	return Ret_Flag;
}





//----------------------------------------------------------------------------------------------
//            TITLE   : KbdExt_ReadKey_Map
//
//            WORK    : 
//            
//            DATE    : 2004. 3. 19
//----------------------------------------------------------------------------------------------
S16     KbdExt_ReadKey_Map( char *Map_File )
{
	S16		Meta_Flag;
	S16             Colum_Cnt;

	long            KeyCode_ID;
	long		KeyCode;
	S16             LineGet_Flag;
	
	
	FILE		*File_Ptr;
	
	char		Line_Str   [80];    // 한 라인 저장 버퍼
	char		KeyName_Str[80];
	char		Flag_Str   [80];

	char            i;
	char            Opt_Flag;

	// 변수 초기화
	//
	KbdExt_MetaFlag = 0;

	memset( KbdExt_Keys  , 0, sizeof( KbdExt_Keys  ) );
	memset( KbdExt_Metas , 0, sizeof( KbdExt_Metas ) );
	memset( KbdExt_KeyMap, 0, sizeof( KbdExt_KeyMap) );

	memset( Line_Str, 0, sizeof( Line_Str) );


	// 키 맵파일 로드
	//
	File_Ptr = fopen( Map_File, "r" );
	if ( NULL == File_Ptr )
	{
		printf("File not found \n");
		return( RET_ERROR );
	}

	while ( ! feof( File_Ptr ) )
	{

		// 파일에서 데이터를 한줄씩 읽어들여서 처리한다.
		//
		//LineGet_Flag = fgets( Line_Str, sizeof( Line_Str )-1, File_Ptr )

		if ( NULL != fgets( Line_Str, sizeof( Line_Str )-1, File_Ptr ) )
		{

			// 라인 시작이 # 이면  주석 처리
			//
			if ( Line_Str[ 0 ] == '#' ) continue;

			Colum_Cnt = sscanf( Line_Str,"%s %lx", KeyName_Str, &KeyCode_ID );

			if ( Colum_Cnt < 1 ) continue;


			// 메타 문자열 읽어서 처리한다.
			//
			Meta_Flag = KbdExt_CheckMeta( KeyName_Str );

			if ( Meta_Flag != 0 )
			{
				if ( ( Colum_Cnt == 2) && (KeyCode_ID < 128) ) 
				{
					KbdExt_Metas[ KeyCode_ID ] = Meta_Flag;
				}
				
				continue;
			}

			// 옵션 값 설정
			// 
			Opt_Flag = -1;
			for( i=0; i<KBD_EXT_MAX_OPT_STR; i++ )
			{
				if ( strcasecmp( KeyName_Str, KbdExt_OptStr[i] ) == 0 ) 
				{
					Opt_Flag = i;
					
					// KeyCode_ID 를 십진수로 읽기 위해 다시 읽는다
					// 옵션에서만 십진수로 처리하기 위해서..
					//
					sscanf( Line_Str,"%s %d", KeyName_Str, &KeyCode_ID );									
					break;
				}
			}
			switch( Opt_Flag )
			{
				case KBD_EXT_OPT_KEY_FIRST:
					KbdExt_KeyRepeat_First = KeyCode_ID;
					continue;				
					break;
				case KBD_EXT_OPT_KEY_NEXT:
					KbdExt_KeyNext = KeyCode_ID;
					continue;				
					break;
			}		
			

			// 메타 문자열 , 옵션 값 다 채크하고 매핑 데이터 인지 아닌지 검사
			//
			if ( strlen( KeyName_Str ) != 2 )       continue;
			if ( (! isxdigit( KeyName_Str[0]))  )   continue;
			if ( (! isxdigit( KeyName_Str[1]))  )   continue;
			

			Colum_Cnt = sscanf( Line_Str, "%lx %s %s %lx", 
			                              &KeyCode_ID, KeyName_Str, Flag_Str, &KeyCode );


			//printf("%lx %s %s %lx\n",KeyCode_ID, KeyName_Str, Flag_Str, KeyCode );
			

			if ( Colum_Cnt < 4 )    continue;

			if ( KeyCode_ID > 127 ) continue;

			Meta_Flag = 0;


			// 메타키가 설정되어있으면 메타키 배열에 할당한다.
			//
			// S C F C O 에 해당하는것
			//
			for( i=0; i<KBD_EXT_MAX_META_STR; i++ )
			{
				if( strchr( Flag_Str, KbdExt_MetaChr[i] ) != NULL )
				{
					Meta_Flag |= (1<<i);
				}
			}


			KbdExt_KeyMap[ Meta_Flag ][ KeyCode_ID ].Key_Name = strdup( KeyName_Str );
			KbdExt_KeyMap[ Meta_Flag ][ KeyCode_ID ].Key_Code = KeyCode;
			

		}
        }

	fclose( File_Ptr );


	printf("Key First : %d \n", KbdExt_KeyRepeat_First );
	printf("Key Next  : %d \n", KbdExt_KeyNext );
	return  RET_OK;
}








//----------------------------------------------------------------------------------------------
//            TITLE   : KbdExt_Close
//
//            WORK    : 
//            
//            DATE    : 2004. 3. 19
//----------------------------------------------------------------------------------------------
void    KbdExt_Close( void )
{
	int	i;
	long	KeyCode;

	
	// 키가 눌렸다는 신호가 있는거는 모두 소거 시킨다. 혹시 모르니까
	//
	for( i = 0; i < 127; i++ ) 
	{
		if ( KbdExt_Keys[ i ] != 0 ) 
		{
			KeyCode = KbdExt_KeyMap[ 0 ][ i ].Key_Code & 0x2a000000;
			
			if ( KeyCode != 0 ) 
			{
				 KbdSharp_Putch( KeyCode );
			}
		}
	}
	
}




