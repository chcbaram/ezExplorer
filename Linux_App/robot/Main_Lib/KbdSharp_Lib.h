//----------------------------------------------------------------------------------------------
//            TITLE       : 
//            WORK        :
//            DATE        : 2003. 3. 7
//            FILE        : KbdSharp_Lib.h
//            Modified By : Hanchol Cho     
//----------------------------------------------------------------------------------------------





#define	KBD_SHARP_DRV	"/dev/sharp_kbdctl"


int KbdSharp_Handle;



U16 KbdSharp_Init( void );
U16 KbdSharp_Putch( long Key );
