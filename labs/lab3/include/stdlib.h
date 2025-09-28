#ifndef _STDLIB_H
#define _STDLIB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <features.h>

#ifdef __cplusplus
#define NULL 0L
#else
#define NULL ((void*)0)
#endif

#define __NEED_size_t
#define __NEED_wchar_t

#include <bits/alltypes.h>

int atoi (const char *);
long atol (const char *);
long long atoll (const char *);
double atof (const char *);

#if defined (__AVR_CONST_DATA_IN_MEMX_ADDRESS_SPACE__)
float strtof (const char *__restrict, const char **__restrict);
double strtod (const char *__restrict, const char **__restrict);
long double strtold (const char *__restrict, const char **__restrict);
#else
float strtof (const char *__restrict, char **__restrict);
double strtod (const char *__restrict, char **__restrict);
long double strtold (const char *__restrict, char **__restrict);
#endif

#if defined (__AVR_CONST_DATA_IN_MEMX_ADDRESS_SPACE__)
long strtol (const char *__restrict, const char **__restrict, int);
unsigned long strtoul (const char *__restrict, const char **__restrict, int);
long long strtoll (const char *__restrict, const char **__restrict, int);
unsigned long long strtoull (const char *__restrict, const char **__restrict, int);
#else
long strtol (const char *__restrict, char **__restrict, int);
unsigned long strtoul (const char *__restrict, char **__restrict, int);
long long strtoll (const char *__restrict, char **__restrict, int);
unsigned long long strtoull (const char *__restrict, char **__restrict, int);
#endif

int rand (void);
void srand (unsigned);

extern char *__malloc_heap_start;
extern char *__malloc_heap_end;

void *malloc (size_t);
void *calloc (size_t, size_t);
void *realloc (void *, size_t);
void free (void *);
void *aligned_alloc(size_t, size_t);

_Noreturn void abort (void);
int atexit (void (*) (void));
_Noreturn void exit (int);
_Noreturn void _Exit (int);
/* Exit hook for simulators to handle exit code
   This will be called from exit, just before _Exit.  */
void __simulator_exit (int ec);

void *bsearch (const void *, const void *, size_t, size_t, int (*)(const void *, const void *));
void qsort (void *, size_t, size_t, int (*)(const void *, const void *));

int abs (int);
long labs (long);
long long llabs (long long);

#define abs(__i) __builtin_abs(__i)
#define labs(__i) __builtin_labs(__i)
#define llabs(__i) __builtin_llabs(__i)

typedef struct { int quot, rem; } div_t;
typedef struct { long quot, rem; } ldiv_t;
typedef struct { long long quot, rem; } lldiv_t;

div_t div (int, int);
ldiv_t ldiv (long, long);
lldiv_t lldiv (long long, long long);

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#define MB_CUR_MAX 1

#define RAND_MAX (0x7fff)

#ifdef __cplusplus
}
#endif

#endif
