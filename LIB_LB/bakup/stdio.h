#ifndef _STDIO__HEADER_
#define _STDIO__HEADER_

#include <stdarg.h>
//#include <stdint.h>

typedef unsigned int		__kernel_size_t;
typedef unsigned short		__kernel_mode_t;
typedef long		        __kernel_time_t;

#ifndef	COMPILE_GCC
 #define inline		__inline
#endif	/* !COMPILE_GCC */

typedef __kernel_size_t		size_t;
typedef __kernel_mode_t		mode_t;
typedef __kernel_time_t		time_t;

#define CL_SCREEN  "\033[2J"
#define CL_HOME "\033[H"
#define CL_WHITE "\033[29m"
#define CL_GRAY  "\033[30m"
#define CL_RED  "\033[31m"
#define CL_GREEN "\033[32m"
#define CL_YELLOW "\033[33m"
#define CL_BLUE  "\033[34m"
#define CL_MAGENTA "\033[35m"
#define CL_CYAN  "\033[36m"
#define CL_RESET "\033[m"

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef BOOL
typedef unsigned int BOOL;
#endif

#ifndef FILE
typedef void FILE;
#endif

#define        NUL        0x00
#define        SOH        0x01
#define        STX        0x02
#define        ETX        0x03
#define        EOT        0x04
#define        ENQ        0x05
#define        ACK        0x06
#define        BEL        0x07
#define        BS         0x08
#define        HT         0x09
#define        LF         0x0a
#define        VT         0x0b
#define        FF         0x0c
#define        CR_        0x0d
#define        SO         0x0e
#define        SI         0x0f
#define        DLE        0x10
#define        DC1        0x11
#define        DC2        0x12
#define        DC3        0x13
#define        DC4        0x14
#define        NAK        0x15
#define        SYN        0x16
#define        ETB        0x17
#define        CAN        0x18
#define        EM         0x19
#define        SUB        0x1a
#define        ESC        0x1b
#define        FS         0x1c
#define        GS         0x1d
#define        RS         0x1e
#define        US         0x1f
#define        DEL        0x7f

#define _U	0x01	/* upper */
#define _L	0x02	/* lower */
#define _D	0x04	/* digit */
#define _C	0x08	/* cntrl */
#define _P	0x10	/* punct */
#define _S	0x20	/* white space (space/lf/tab) */
#define _X	0x40	/* hex digit */
#define _SP	0x80	/* hard space (0x20) */

extern unsigned char _ctype[];

#define __ismask(x) (_ctype[(int)(unsigned char)(x)])

#define isalnum(c)	((__ismask(c)&(_U|_L|_D)) != 0)
#define isalpha(c)	((__ismask(c)&(_U|_L)) != 0)
#define iscntrl(c)	((__ismask(c)&(_C)) != 0)
#define isdigit(c)	((__ismask(c)&(_D)) != 0)
#define isgraph(c)	((__ismask(c)&(_P|_U|_L|_D)) != 0)
#define islower(c)	((__ismask(c)&(_L)) != 0)
#define isprint(c)	((__ismask(c)&(_P|_U|_L|_D|_SP)) != 0)
#define ispunct(c)	((__ismask(c)&(_P)) != 0)
#define isspace(c)	((__ismask(c)&(_S)) != 0)
#define isupper(c)	((__ismask(c)&(_U)) != 0)
#define isxdigit(c)	((__ismask(c)&(_D|_X)) != 0)

#define isascii(c) (((unsigned char)(c))<=0x7f)
#define toascii(c) (((unsigned char)(c))&0x7f)

static inline unsigned char __tolower(unsigned char c)
{
	if (isupper(c))
		c -= 'A'-'a';
	return c;
}

static inline unsigned char __toupper(unsigned char c)
{
	if (islower(c))
		c -= 'a'-'A';
	return c;
}

#define tolower(c) __tolower(c)
#define toupper(c) __toupper(c)


#define ZEROPAD	1		/* pad with zero */
#define SIGN	2		/* unsigned/signed long */
#define PLUS	4		/* show plus */
#define SPACE	8		/* space if plus */
#define LEFT	16		/* left justified */
#define SPECIAL	32		/* 0x */
#define LARGE	64		/* use 'ABCDEF' instead of 'abcdef' */

#if 0 /* def COMPILE_GCC */
#error Replaced with function, macro may be incompatible with the latest source
#define do_div(n,base)						\
({								\
	int __res;						\
	__res = ((unsigned long)n) % (unsigned int)base;	\
	n = ((unsigned long)n) / (unsigned int)base;		\
	__res;							\
})
#endif
//static int do_div ( int *n, unsigned int base );
#define printf	cmprintf

extern int printf(const char *fmt, ...);

extern int sprintf(char * buf, const char *fmt, ...);
extern void delay_us(unsigned int usec);
extern void osa_mdelay(unsigned int msec);

#if 0
extern int putc( char c);

extern int getc(void);
extern int gets(char *s);

extern int getc_timed( char cmpKey, int sec );
extern int getc_mtimed( char cmpKey, int msec );

extern int vsscanf(char *s, const char *fmt, va_list args);
extern int sscanf(char *s, const char *fmt, ...);
#endif

#endif //_STDIO_HEADER_
