/*!
  * @file    common/string.h
  * @brief   string Control API
  */


#ifndef _STRING_HEADER_
#define _STRING_HEADER_


#include "Hw.h"

//extern char * ___strtok;
/**
  * @brief  string copy
  */
char * strcpy(char * dest,const char *src);

/**
  * @brief  limited length string copy
  */
char * strncpy(char * dest,const char *src,size_t count);

/**
  * @brief  append string
  */
char * strcat(char * dest, const char * src);

/**
  * @brief  append limited length string
  */
char * strncat(char *dest, const char *src, size_t count);

/**
  * @brief  compare string
  */
int strcmp(const char * cs,const char * ct);

/**
  * @brief  Compare String. Ignore Capital
  */
int strnicmp(const char *s1, const char *s2, size_t len);

/**
  * @brief  compare limited length string.
  */
int strncmp(const char * cs,const char * ct,size_t count);

/**
  * @brief  calculate length of string
  */
size_t strlen(const char * s);

/**
  * @brief  calculate length of limited length string 
  */
size_t strnlen(const char * s, size_t count);

/**
  * @brief  find a first letter in string
  */
char * strchr(const char * s, int c);

/**
  * @brief  find a last letter in string
  */
char * strrchr(const char * s, int c);

/**
  * @brief  find string in source string
  */
size_t strspn(const char *s, const char *accept);
;

/**
  * @brief  find string position with one of separating characters
  */
char * strpbrk(const char * cs,const char * ct);

/**
  * @brief  cut string with one of seprating characters
  */
char * strtok(char * s,const char * ct);

/**
  * @brief  cut string with one of seprating characters
  */
char * strsep(char **s, const char *ct);

/**
  * @brief  fill data to specific memory area
  */
void * memset(void * s,int c,size_t count);

/**
  * @brief  memory copy
  */
void bcopy(void * src, void *dest, size_t count);

/**
  * @brief  memory copy
  */
void * memcpy(void * dest,const void *src,size_t count);

/**
  * @brief  memory copy
  */
void * memmove(void * dest,const void *src,size_t count);

/**
  * @brief  compare memory
  */
int memcmp(const void * cs,const void * ct,size_t count);

/**
  * @brief  memory scan
  */
void * memscan(void * addr, int c, size_t size);

/**
  * @brief  find string in string
  */
char * strstr(const char * s1,const char * s2);

/**
  * @brief  find first 1 byte data in memory
  */
void *memchr(const void *s, int c, size_t n);


/**
  * @brief  change to Capital letter
  */
void UpperStr( char *Str );

/**
  * @brief  change to small letter
  */
void LowerStr( char *Str );

/**
  * @brief  change numeric string to integer
  */
unsigned long strtoul(const char *p, char **out_p, int base);


#endif // _STRING_HEADER_

