/*!
  * @file    common/vsprintf.c
  * @brief   string Control API
 */

#include "stdio.h"
#include <string.h>
#include "Lb_Printf.h"


//-- 외부 포팅 함수
//
extern void print_byte(unsigned int c);
extern char get_byte(void);


char SerialGetChar( void )
{
	return get_byte();
}

void SerialPutCharCR(char Ch)
{
	print_byte(Ch);
}


void SerialPutChar(char Ch)
{
	print_byte(Ch);
}


void SerialPutStringCR(char *str, int len)
{
	int	lp ;
	for (lp=0;lp<len;lp++)
		SerialPutCharCR(str[lp]);
}



static int skip_atoi(const char **s)
{
	int i=0;

	while (isdigit(**s))	i = i*10 + *((*s)++) - '0';
	return i;
}

#define do_div(n,base) ({ \
	int __res; \
	__res = ((unsigned long) n) % (unsigned) base; \
	n = ((unsigned long) n) / (unsigned) base; \
	__res; \
})

static char * number(char * str, long long num, int base, int size, int precision, int type)
{
	char c,sign,tmp[66];
	const char *digits="0123456789abcdefghijklmnopqrstuvwxyz";
	int i;

	if (type & LARGE)
		digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	if (type & LEFT)
		type &= ~ZEROPAD;
	if (base < 2 || base > 36)
		return 0;
	c = (type & ZEROPAD) ? '0' : ' ';
	sign = 0;
	if (type & SIGN) {
		if (num < 0) {
			sign = '-';
			num = -num;
			size--;
		} else if (type & PLUS) {
			sign = '+';
			size--;
		} else if (type & SPACE) {
			sign = ' ';
			size--;
		}
	}
	if (type & SPECIAL) {
		if (base == 16)
			size -= 2;
		else if (base == 8)
			size--;
	}
	i = 0;
	if (num == 0)
		tmp[i++]='0';
#if 0
	else while (num != 0)
		tmp[i++] = digits[do_div(num,base)];
#else
	else
	{
		while ( num != 0 )
		{
			tmp[i++] = digits[((unsigned long) num) % (unsigned) base];
			num = ((unsigned long) num) / (unsigned) base;
		}
	}
#endif	/* #if 0 */
	if (i > precision)
		precision = i;
	size -= precision;
	if (!(type&(ZEROPAD+LEFT)))
		while(size-->0)
			*str++ = ' ';
	if (sign)
		*str++ = sign;
	if (type & SPECIAL) {
		if (base==8)
			*str++ = '0';
		else if (base==16) {
			*str++ = '0';
			*str++ = digits[33];
		}
	}
	if (!(type & LEFT))
		while (size-- > 0)
			*str++ = c;
	while (i < precision--)
		*str++ = '0';
	while (i-- > 0)
		*str++ = tmp[i];
	while (size-- > 0)
		*str++ = ' ';
	return str;
}

/*!
  @brief	get string from uart (when received CR)
  @param	[out] s	string buffer
  @return	number of string length
 */
int gets(char *s)
{
	int cnt = 0;
	char c;
	char strbb[] = "\b \b";
	while((c = SerialGetChar()) != CR_)
	{
		if(c != BS){
			cnt++;
			*s++ = c;
			SerialPutCharCR(c);
		}
		else{
			if(cnt > 0){
				cnt--; *s-- = ' ';
				printf("%s", strbb);
			}
		}
	}
	*s = 0;

	return(cnt);
}

/*!
  @brief	put string to uart
  @param	[in] s	string buffer
  @return	none
 */
int puts(char *s)
{
	while(*s != '\0')
	{
		SerialPutCharCR(*s);
		s++;
	}

	return 0;
}

#if 0
size_t strnlen(const char * s, size_t count)
{
	const char *sc;

	for (sc = s; count-- && *sc != '\0'; ++sc)
		/* nothing */;
	return sc - s;
}
#endif

static int cmvsprintf(char *buf, const char *fmt, va_list args)
{
	int len;
	unsigned long num;
	int i, base;
	char * str;
	const char *s;

	int flags;		/* flags to number() */

	int field_width;	/* width of output field */
	int precision;		/* min. # of digits for integers; max
				   number of chars for from string */
	int qualifier;		/* 'h', 'l', or 'L' for integer fields */

	for (str=buf ; *fmt ; ++fmt) {
		if (*fmt != '%') {
			*str++ = *fmt;
			continue;
		}

		/* process flags */
		flags = 0;
		repeat:
			++fmt;		/* this also skips first '%' */
			switch (*fmt) {
				case '-': flags |= LEFT; goto repeat;
				case '+': flags |= PLUS; goto repeat;
				case ' ': flags |= SPACE; goto repeat;
				case '#': flags |= SPECIAL; goto repeat;
				case '0': flags |= ZEROPAD; goto repeat;
				}

		/* get field width */
		field_width = -1;
		if (isdigit(*fmt))
			field_width = skip_atoi(&fmt);
		else if (*fmt == '*') {
			++fmt;
			/* it's the next argument */
			field_width = va_arg(args, int);
			if (field_width < 0) {
				field_width = -field_width;
				flags |= LEFT;
			}
		}

		/* get the precision */
		precision = -1;
		if (*fmt == '.') {
			++fmt;
			if (isdigit(*fmt))
				precision = skip_atoi(&fmt);
			else if (*fmt == '*') {
				++fmt;
				/* it's the next argument */
				precision = va_arg(args, int);
			}
			if (precision < 0)
				precision = 0;
		}

		/* get the conversion qualifier */
		qualifier = -1;
//		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L')
		if (*fmt == 'l' || *fmt == 'L')
		{
			qualifier = *fmt;
			++fmt;
		}

		/* default base */
		base = 10;

		switch (*fmt) {
		case 'c':
			if (!(flags & LEFT))
				while (--field_width > 0)
					*str++ = ' ';
			*str++ = (unsigned char) va_arg(args, int);
			while (--field_width > 0)
				*str++ = ' ';
			continue;

		case 's':
			s = va_arg(args, char *);
			if (!s)
				s = "<NULL>";

			len = strnlen(s, precision);

			if (!(flags & LEFT))
				while (len < field_width--)
					*str++ = ' ';
			for (i = 0; i < len; ++i)
				*str++ = *s++;
			while (len < field_width--)
				*str++ = ' ';
			continue;

		case 'p':
			if (field_width == -1) {
				field_width = 2*sizeof(void *);
				flags |= ZEROPAD;
			}
			str = number(str,
				(unsigned long) va_arg(args, void *), 16,
				field_width, precision, flags);
			continue;


		case 'n':
			if (qualifier == 'l') {
				long * ip = va_arg(args, long *);
				*ip = (str - buf);
			} else {
				int * ip = va_arg(args, int *);
				*ip = (str - buf);
			}
			continue;

		/* integer number formats - set up the flags and "break" */
		case 'o':
			base = 8;
			break;

		case 'X':
			flags |= LARGE;
		case 'x':
			base = 16;
			break;

		case 'd':
		case 'i':
			flags |= SIGN;
		case 'u':
			break;

		default:
			if (*fmt != '%')
				*str++ = '%';
			if (*fmt)
				*str++ = *fmt;
			else
				--fmt;
			continue;
		}
		if (qualifier == 'l')
			num = va_arg(args, unsigned long);
#if 0
		else if (qualifier == 'h') {
			if (flags & SIGN)
				num = va_arg(args, short);
			else
				num = va_arg(args, unsigned short);
		}
#endif
		else if (flags & SIGN)
			num = va_arg(args, int);
		else
			num = va_arg(args, unsigned int);
		str = number(str, num, base, field_width, precision, flags);
	}
	*str = '\0';
	return str-buf;
}

int Lb_sprintf(char *pStr, char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i=cmvsprintf(pStr,fmt,args);
	va_end(args);
	return i;
}

int Lb_printf(char *fmt, ...)
{
	char buffer[2048];
	va_list ap;
	int len;

	va_start(ap, fmt);
	len = cmvsprintf(buffer, fmt, ap);
	va_end(ap);

	SerialPutStringCR(buffer, len);
	return len;
}


int Lb_vsprintf( char *pStr, char *fmt, Lb_va_list args )
{
	return cmvsprintf(pStr, fmt, args);
}
