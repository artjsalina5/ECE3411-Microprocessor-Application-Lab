#ifndef _INTTYPES_H
#define _INTTYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <features.h>
#include <stdint.h>

/* integer types that can hold pointer > 64 KB  */
typedef int32_t int_farptr_t;
typedef uint32_t uint_farptr_t;

#define __NEED_wchar_t
#include <bits/alltypes.h>

typedef struct { intmax_t quot, rem; } imaxdiv_t;

intmax_t imaxabs(intmax_t);
imaxdiv_t imaxdiv(intmax_t, intmax_t);

#if defined (__AVR_CONST_DATA_IN_MEMX_ADDRESS_SPACE__)
intmax_t strtoimax(const char *__restrict, const char **__restrict, int);
uintmax_t strtoumax(const char *__restrict, const char **__restrict, int);
#else
intmax_t strtoimax(const char *__restrict, char **__restrict, int);
uintmax_t strtoumax(const char *__restrict, char **__restrict, int);
#endif

intmax_t wcstoimax(const wchar_t *__restrict, wchar_t **__restrict, int);
uintmax_t wcstoumax(const wchar_t *__restrict, wchar_t **__restrict, int);

#ifndef __PRI32
#define __PRI32 "l"
#endif

#ifndef __PRI64
#define __PRI64 "ll"
#endif

#ifndef __PRIPTR
#define __PRIPTR "l"
#endif

#define PRId8  "d"
#define PRId16 "d"
#define PRId32 __PRI32 "d"
#define PRId64 __PRI64 "d"

#define PRIdLEAST8  "d"
#define PRIdLEAST16 "d"
#define PRIdLEAST32 __PRI32 "d"
#define PRIdLEAST64 __PRI64 "d"

#define PRIdFAST8  "d"
#define PRIdFAST16 "d"
#define PRIdFAST32 __PRI32 "d"
#define PRIdFAST64 __PRI64 "d"

#define PRIi8  "i"
#define PRIi16 "i"
#define PRIi32 __PRI32 "i"
#define PRIi64 __PRI64 "i"

#define PRIiLEAST8  "i"
#define PRIiLEAST16 "i"
#define PRIiLEAST32 __PRI32 "i"
#define PRIiLEAST64 __PRI64 "i"

#define PRIiFAST8  "i"
#define PRIiFAST16 "i"
#define PRIiFAST32 __PRI32 "i"
#define PRIiFAST64 __PRI64 "i"

#define PRIo8  "o"
#define PRIo16 "o"
#define PRIo32 __PRI32 "o"
#define PRIo64 __PRI64 "o"

#define PRIoLEAST8  "o"
#define PRIoLEAST16 "o"
#define PRIoLEAST32 __PRI32 "o"
#define PRIoLEAST64 __PRI64 "o"

#define PRIoFAST8  "o"
#define PRIoFAST16 "o"
#define PRIoFAST32 __PRI32 "o"
#define PRIoFAST64 __PRI64 "o"

#define PRIu8  "u"
#define PRIu16 "u"
#define PRIu32 __PRI32 "u"
#define PRIu64 __PRI64 "u"

#define PRIuLEAST8  "u"
#define PRIuLEAST16 "u"
#define PRIuLEAST32 __PRI32 "u"
#define PRIuLEAST64 __PRI64 "u"

#define PRIuFAST8  "u"
#define PRIuFAST16 "u"
#define PRIuFAST32 __PRI32 "u"
#define PRIuFAST64 __PRI64 "u"

#define PRIx8  "x"
#define PRIx16 "x"
#define PRIx32 __PRI32 "x"
#define PRIx64 __PRI64 "x"

#define PRIxLEAST8  "x"
#define PRIxLEAST16 "x"
#define PRIxLEAST32 __PRI32 "x"
#define PRIxLEAST64 __PRI64 "x"

#define PRIxFAST8  "x"
#define PRIxFAST16 "x"
#define PRIxFAST32 __PRI32 "x"
#define PRIxFAST64 __PRI64 "x"

#define PRIX8  "X"
#define PRIX16 "X"
#define PRIX32 __PRI32 "X"
#define PRIX64 __PRI64 "X"

#define PRIXLEAST8  "X"
#define PRIXLEAST16 "X"
#define PRIXLEAST32 __PRI32 "X"
#define PRIXLEAST64 __PRI64 "X"

#define PRIXFAST8  "X"
#define PRIXFAST16 "X"
#define PRIXFAST32 __PRI32 "X"
#define PRIXFAST64 __PRI64 "X"

#define PRIdMAX __PRI64 "d"
#define PRIiMAX __PRI64 "i"
#define PRIoMAX __PRI64 "o"
#define PRIuMAX __PRI64 "u"
#define PRIxMAX __PRI64 "x"
#define PRIXMAX __PRI64 "X"

#define PRIdPTR __PRIPTR "d"
#define PRIiPTR __PRIPTR "i"
#define PRIoPTR __PRIPTR "o"
#define PRIuPTR __PRIPTR "u"
#define PRIxPTR __PRIPTR "x"
#define PRIXPTR __PRIPTR "X"

#define SCNd8   "hhd"
#define SCNd16  "hd"
#define SCNd32  __PRI32 "d"
#define SCNd64  __PRI64 "d"

#define SCNdLEAST8  "hhd"
#define SCNdLEAST16 "hd"
#define SCNdLEAST32 __PRI32 "d"
#define SCNdLEAST64 __PRI64 "d"

#define SCNdFAST8  "hhd"
#define SCNdFAST16 "d"
#define SCNdFAST32 __PRI32 "d"
#define SCNdFAST64 __PRI64 "d"

#define SCNi8   "hhi"
#define SCNi16  "hi"
#define SCNi32  __PRI32 "i"
#define SCNi64  __PRI64 "i"

#define SCNiLEAST8  "hhi"
#define SCNiLEAST16 "hi"
#define SCNiLEAST32 __PRI32 "i"
#define SCNiLEAST64 __PRI64 "i"

#define SCNiFAST8  "hhi"
#define SCNiFAST16 "i"
#define SCNiFAST32 __PRI32 "i"
#define SCNiFAST64 __PRI64 "i"

#define SCNu8   "hhu"
#define SCNu16  "hu"
#define SCNu32  __PRI32 "u"
#define SCNu64  __PRI64 "u"

#define SCNuLEAST8  "hhu"
#define SCNuLEAST16 "hu"
#define SCNuLEAST32 __PRI32 "u"
#define SCNuLEAST64 __PRI64 "u"

#define SCNuFAST8 "hhu"
#define SCNuFAST16 "u"
#define SCNuFAST32 __PRI32 "u"
#define SCNuFAST64 __PRI64 "u"

#define SCNo8   "hho"
#define SCNo16  "ho"
#define SCNo32  __PRI32 "o"
#define SCNo64  __PRI64 "o"

#define SCNoLEAST8  "hho"
#define SCNoLEAST16 "ho"
#define SCNoLEAST32 __PRI32 "o"
#define SCNoLEAST64 __PRI64 "o"

#define SCNoFAST8  "hho"
#define SCNoFAST16 "o"
#define SCNoFAST32 __PRI32 "o"
#define SCNoFAST64 __PRI64 "o"

#define SCNx8   "hhx"
#define SCNx16  "hx"
#define SCNx32  __PRI32 "x"
#define SCNx64  __PRI64 "x"

#define SCNxLEAST8  "hhx"
#define SCNxLEAST16 "hx"
#define SCNxLEAST32 __PRI32 "x"
#define SCNxLEAST64 __PRI64 "x"

#define SCNxFAST8  "hhx"
#define SCNxFAST16 "x"
#define SCNxFAST32 __PRI32 "x"
#define SCNxFAST64 __PRI64 "x"

#define SCNdMAX __PRI64 "d"
#define SCNiMAX __PRI64 "i"
#define SCNoMAX __PRI64 "o"
#define SCNuMAX __PRI64 "u"
#define SCNxMAX __PRI64 "x"

#define SCNdPTR __PRIPTR "d"
#define SCNiPTR __PRIPTR "i"
#define SCNoPTR __PRIPTR "o"
#define SCNuPTR __PRIPTR "u"
#define SCNxPTR __PRIPTR "x"

#ifdef __cplusplus
}
#endif

#endif

