#ifndef _STDARG_HEADER_
#define _STDARG_HEADER_

typedef void *va_list;

#  define va_start(ap, p)	(ap = (va_list) (&(p)+1))
#define va_arg(AP, TYPE)						\
		(AP = (va_list) ((char *) (AP) + sizeof(TYPE)),	\
		*((TYPE *) (void *) ((char *) (AP) - sizeof(TYPE))))
#  define va_end(ap)		((void)0)

#endif // _STDARG_HEADER_
