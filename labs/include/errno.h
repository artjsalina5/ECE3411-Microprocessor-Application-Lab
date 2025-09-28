#ifndef	_ERRNO_H
#define _ERRNO_H

#ifdef __cplusplus
extern "C" {
#endif

extern int __errno_val;
#define errno (__errno_val)

#define EINTR       4  /* Interrupted system call */
#define ENOMEM     12  /* Out of memory */
#define EINVAL     22  /* Invalid argument */
#define EDOM       33  /* Domain error */
#define ERANGE     34  /* Range error */
#define EOVERFLOW  79  /* Value too large for data type */
#define EILSEQ     84  /* Illegal byte sequence */
#define ENOSYS     89  /* Function not implemented */

#ifdef __cplusplus
}
#endif

#endif
