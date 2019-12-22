// $Id: liboil.h,v 1.2 2019-10-16 15:44:18-07 - - $

#ifndef __LIBOIL_H__
#define __LIBOIL_H__

typedef __SIZE_TYPE__ size_t;

int printf (const char* fmt, ...);
int putchar (int c);
int puts (const char* s);
void exit (int status);
void free (void* ptr);
void* malloc (size_t size);

#endif

