#ifndef _MATH_H
#define _MATH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <features.h>

#define __NEED_float_t
#define __NEED_double_t
#include <bits/alltypes.h>

#if 100*__GNUC__+__GNUC_MINOR__ >= 303
#define NAN       __builtin_nanf("")
#define INFINITY  __builtin_inff()
#else
#define NAN       (0.0f/0.0f)
#define INFINITY  1e5000f
#endif

#define HUGE_VALF INFINITY
#define HUGE_VAL  ((double)INFINITY)
#define HUGE_VALL ((long double)INFINITY)

#define MATH_ERRNO  1
#define MATH_ERREXCEPT 2
#define math_errhandling 1

#define FP_ILOGBNAN (-1-(int)(((unsigned)-1)>>1))
#define FP_ILOGB0 FP_ILOGBNAN

#define FP_NAN       0
#define FP_INFINITE  1
#define FP_ZERO      2
#define FP_SUBNORMAL 3
#define FP_NORMAL    4

int __fpclassify(double);
int __fpclassifyf(float);
int __fpclassifyl(long double);

static __inline uint32_t __FLOAT_BITS(float __f)
{
	union {float __f; uint32_t __i;} __u;
	__u.__f = __f;
	return __u.__i;
}
static __inline uint64_t __DOUBLE_BITS(double __f)
{
	union {double __f; uint64_t __i;} __u;
	__u.__f = __f;
	return __u.__i;
}

#define fpclassify(x) __fpclassifyf(x)

#define isinf(x) ((__FLOAT_BITS(x) & 0x7fffffff) == 0x7f800000)

#define isnan(x) ((__FLOAT_BITS(x) & 0x7fffffff) > 0x7f800000)

#define isnormal(x) (((__FLOAT_BITS(x)+0x00800000) & 0x7fffffff) >= 0x01000000)

#define isfinite(x) ((__FLOAT_BITS(x) & 0x7fffffff) < 0x7f800000)

int __signbit(double);
int __signbitf(float);
int __signbitl(long double);

#define signbit(x) ((int32_t)__FLOAT_BITS(x)>>31)

#define isunordered(x,y) (isnan((x)) ? ((void)(y),1) : isnan((y)))

#define __ISREL_DEF(rel, op, type) \
static __inline int __is##rel(type __x, type __y) \
{ return !isunordered(__x,__y) && __x op __y; }

__ISREL_DEF(lessf, <, float_t)
__ISREL_DEF(less, <, double_t)
__ISREL_DEF(lessl, <, long double)
__ISREL_DEF(lessequalf, <=, float_t)
__ISREL_DEF(lessequal, <=, double_t)
__ISREL_DEF(lessequall, <=, long double)
__ISREL_DEF(lessgreaterf, !=, float_t)
__ISREL_DEF(lessgreater, !=, double_t)
__ISREL_DEF(lessgreaterl, !=, long double)
__ISREL_DEF(greaterf, >, float_t)
__ISREL_DEF(greater, >, double_t)
__ISREL_DEF(greaterl, >, long double)
__ISREL_DEF(greaterequalf, >=, float_t)
__ISREL_DEF(greaterequal, >=, double_t)
__ISREL_DEF(greaterequall, >=, long double)

#define __tg_pred_2(x, y, p) ( \
	sizeof((x)+(y)) == sizeof(float) ? p##f(x, y) : \
	sizeof((x)+(y)) == sizeof(double) ? p(x, y) : \
	p##l(x, y) )

#define isless(x, y)            __tg_pred_2(x, y, __isless)
#define islessequal(x, y)       __tg_pred_2(x, y, __islessequal)
#define islessgreater(x, y)     __tg_pred_2(x, y, __islessgreater)
#define isgreater(x, y)         __tg_pred_2(x, y, __isgreater)
#define isgreaterequal(x, y)    __tg_pred_2(x, y, __isgreaterequal)

float    acosf(float);
#define  acos acosf
#define  acosl acosf

float    acoshf(float);
#define  acosh acoshf
#define  acoshl acoshf

float    asinf(float);
#define  asin asinf
#define  asinl asinf

float    asinhf(float);
#define  asinh asinhf
#define  asinhl asinhf

float    atanf(float);
#define  atan atanf
#define  atanl atanf

float    atan2f(float, float);
#define  atan2 atan2f
#define  atan2l atan2f

float    atanhf(float);
#define  atanh atanhf
#define  atanhl atanhf

float    cbrtf(float);
#define  cbrt cbrtf
#define  cbrtl cbrtf

float       ceilf(float);
double      ceil(double);
long double ceill(long double);

float    copysignf(float, float);
#define  copysign copysignf
#define  copysignl copysignf

float    cosf(float);
#define  cos cosf
#define  cosl cosf

float    coshf(float);
#define  cosh coshf
#define  coshl coshf

float    erff(float);
#define  erf erff
#define  erfl erff

float    erfcf(float);
#define  erfc erfcf
#define  erfcl erfcf

float    expf(float);
#define  exp expf
#define  expl expf

float    exp2f(float);
#define  exp2 exp2f
#define  exp2l exp2f

float    expm1f(float);
#define  expm1 expm1f
#define  expm1l expm1f

float    fabsf(float);
#define  fabs fabsf
#define  fabsl fabsf

float    fdimf(float, float);
#define  fdim fdimf
#define  fdiml fdimf

float    floorf(float);
#define  floor floorf
#define  floorl floorf

float    fmaf(float, float, float);
#define  fma fmaf
#define  fmal fmaf

float    fmaxf(float, float);
#define  fmax fmaxf
#define  fmaxl fmaxf

float    fminf(float, float);
#define  fmin fminf
#define  fminl fminf

float    fmodf(float, float);
#define  fmod fmodf
#define  fmodl fmodf

float    frexpf(float, int *);
#define  frexp frexpf
#define  frexpl frexpf

float    hypotf(float, float);
#define  hypot hypotf
#define  hypotl hypotf

int      ilogbf(float);
#define  ilogb ilogbf 
#define  ilogbl ilogbf

float    ldexpf(float, int);
#define  ldexp ldexpf
#define  ldexpl ldexpf

float    lgammaf(float);
#define  lgamma lgammaf
#define  lgammal lgammaf

long long  llrintf(float);
#define    llrint llrintf
#define    llrintl llrintf

long long  llroundf(float);
#define    llround llroundf
#define    llroundl llroundf

float    logf(float);
#define  log logf
#define  logl logf

float    log10f(float);
#define  log10 log10f
#define  log10l log10f

float    log1pf(float);
#define  log1p log1pf
#define  log1pl log1pf

float    log2f(float);
#define  log2 log2f
#define  log2l log2f

float    logbf(float);
#define  logb logbf
#define  logbl logbf

long     lrintf(float);
#define  lrint lrintf
#define  lrintl lrintf

long     lroundf(float);
#define  lround lroundf
#define  lroundl lroundf

float       modff(float, float *);
double      modf(double, double *);
long double modfl(long double, long double *);

float    nanf(const char *);
#define  nan nanf
#define  nanl nanf

float    nearbyintf(float);
#define  nearbyint nearbyintf
#define  nearbyintl nearbyintf

float    nextafterf(float, float);
#define  nextafter nextafterf
#define  nextafterl nextafterf

float    nexttowardf(float, long double);
#define  nexttoward nexttowardf
#define  nexttowardl nexttowardf

float       powf(float, float);
double      pow(double, double);
long double powl(long double, long double);

float    remainderf(float, float);
#define  remainder remainderf
#define  remainderl remainderf

float    remquof(float, float, int *);
#define  remquo remquof
#define  remquol remquof

float    rintf(float);
#define  rint rintf
#define  rintl rintf

float    roundf(float);
#define  round roundf
#define  roundl roundf

float    scalblnf(float, long);
#define  scalbln scalblnf
#define  scalblnl scalblnf

float    scalbnf(float, int);
#define  scalbn scalbnf
#define  scalbnl scalbnf

float    sinf(float);
#define  sin sinf
#define  sinl sinf

float    sinhf(float);
#define  sinh sinhf
#define  sinhl sinhf


#ifndef __ATTR_CONST__
# define __ATTR_CONST__ __attribute__((__const__))
#endif

float       sqrtf(float) __ATTR_CONST__;
double      sqrt(double) __ATTR_CONST__;
long double sqrtl(long double) __ATTR_CONST__;

float    tanf(float);
#define  tan tanf
#define  tanl tanf

float    tanhf(float);
#define  tanh tanhf
#define  tanhl tanhf

float    tgammaf(float);
#define  tgamma tgammaf
#define  tgammal tgammaf

float    truncf(float);
#define  trunc truncf
#define  truncl truncf


#if defined(_XOPEN_SOURCE) || defined(_BSD_SOURCE)
#undef  MAXFLOAT
#define MAXFLOAT        3.40282346638528859812e+38F
#endif

#if defined(_XOPEN_SOURCE) || defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
#define M_E             2.7182818284590452354   /* e */
#define M_LOG2E         1.4426950408889634074   /* log_2 e */
#define M_LOG10E        0.43429448190325182765  /* log_10 e */
#define M_LN2           0.69314718055994530942  /* log_e 2 */
#define M_LN10          2.30258509299404568402  /* log_e 10 */
#define M_PI            3.14159265358979323846  /* pi */
#define M_PI_2          1.57079632679489661923  /* pi/2 */
#define M_PI_4          0.78539816339744830962  /* pi/4 */
#define M_1_PI          0.31830988618379067154  /* 1/pi */
#define M_2_PI          0.63661977236758134308  /* 2/pi */
#define M_2_SQRTPI      1.12837916709551257390  /* 2/sqrt(pi) */
#define M_SQRT2         1.41421356237309504880  /* sqrt(2) */
#define M_SQRT1_2       0.70710678118654752440  /* 1/sqrt(2) */

extern int signgam;

double      j0(double);
double      j1(double);
double      jn(int, double);

double      y0(double);
double      y1(double);
double      yn(int, double);
#endif

#if defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
#define HUGE            3.40282346638528859812e+38F

float       dremf(float, float);
#define     drem dremf

int         finitef(float);
#define     finite finitef

float       scalbf(float, float);
#define     scalb scalbf

float       significandf(float);
#define     significand significandf

float       lgammaf_r(float, int*);
#define     lgamma_r lgammaf_r

float       j0f(float);
float       j1f(float);
float       jnf(int, float);

float       y0f(float);
float       y1f(float);
float       ynf(int, float);
#endif

#ifdef _GNU_SOURCE
long double lgammal_r(long double, int*);

void        sincosf(float, float*, float*);
#define     sincos sincosf
#define     sincosl sincosf

float       exp10f(float);
#define     exp10 exp10f
#define     exp10l exp10f

float       pow10f(float);
#define     pow10 pow10f
#define     pow10l pow10f
#endif

#ifdef __cplusplus
}
#endif

#endif
