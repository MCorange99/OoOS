#ifndef __TYPES
#define __TYPES
#include "kernel/kernel_defs.h"
typedef long ssize_t;
typedef	unsigned short	ushort;		/* System V compatibility */
typedef	unsigned int	uint;		/* System V compatibility */
typedef	unsigned long	ulong;		/* System V compatibility */
typedef long register_t;
typedef __int128_t int128_t;
typedef uint64_t time_t, clock_t, suseconds_t, dev_t, ino_t, nlink_t, uid_t, gid_t, blksize_t, blkcnt_t;
typedef ptrdiff_t off_t;
typedef uint32_t mode_t;
typedef uint64_t pid_t;
#endif