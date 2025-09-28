#ifndef	_LOCALE_H
#define	_LOCALE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <features.h>

#ifdef __cplusplus
#define NULL 0L
#else
#define NULL ((void*)0)
#endif

#define LC_CTYPE    0
#define LC_NUMERIC  1
#define LC_TIME     2
#define LC_COLLATE  3
#define LC_MONETARY 4
#define LC_MESSAGES 5
#define LC_ALL      6

#if defined (__AVR_CONST_DATA_IN_MEMX_ADDRESS_SPACE__)
#define __CONST const
#else
#define __CONST
#endif

struct lconv {
	__CONST char *decimal_point;
	__CONST char *thousands_sep;
	__CONST char *grouping;

	__CONST char *int_curr_symbol;
	__CONST char *currency_symbol;
	__CONST char *mon_decimal_point;
	__CONST char *mon_thousands_sep;
	__CONST char *mon_grouping;
	__CONST char *positive_sign;
	__CONST char *negative_sign;
	char int_frac_digits;
	char frac_digits;
	char p_cs_precedes;
	char p_sep_by_space;
	char n_cs_precedes;
	char n_sep_by_space;
	char p_sign_posn;
	char n_sign_posn;
	char int_p_cs_precedes;
	char int_p_sep_by_space;
	char int_n_cs_precedes;
	char int_n_sep_by_space;
	char int_p_sign_posn;
	char int_n_sign_posn;
};


__CONST char *setlocale (int, const char *);
struct lconv *localeconv(void);


#if defined(_POSIX_SOURCE) || defined(_POSIX_C_SOURCE) \
 || defined(_XOPEN_SOURCE) || defined(_GNU_SOURCE) || defined(_BSD_SOURCE)

#define __NEED_locale_t

#include <bits/alltypes.h>

#define LC_GLOBAL_LOCALE ((locale_t)-1)

#define LC_CTYPE_MASK    (1<<LC_CTYPE)
#define LC_NUMERIC_MASK  (1<<LC_NUMERIC)
#define LC_TIME_MASK     (1<<LC_TIME)
#define LC_COLLATE_MASK  (1<<LC_COLLATE)
#define LC_MONETARY_MASK (1<<LC_MONETARY)
#define LC_MESSAGES_MASK (1<<LC_MESSAGES)
#define LC_ALL_MASK      0x7fffffff

locale_t duplocale(locale_t);
void freelocale(locale_t);
locale_t newlocale(int, const char *, locale_t);
locale_t uselocale(locale_t);

#endif


#ifdef __cplusplus
}
#endif

#endif
