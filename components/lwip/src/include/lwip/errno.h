/**
 * @file
 * Posix Errno defines
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef LWIP_HDR_ERRNO_H
#define LWIP_HDR_ERRNO_H

#include "lwip/opt.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef LWIP_PROVIDE_ERRNO

#define  EPERM            1  /* Operation not permitted */
#define  ENOENT           2  /* No such file or directory */
#define  ESRCH            3  /* No such process */
#define  EINTR            4  /* Interrupted system call */
#define  EIO              5  /* I/O error */
#define  ENXIO            6  /* No such device or address */
#define  E2BIG            7  /* Arg list too long */
#define  ENOEXEC          8  /* Exec format error */
#define  EBADF            9  /* Bad file number */
#define  ECHILD          10  /* No child processes */
#define  EAGAIN          11  /* Try again */
#define  ENOMEM          12  /* Out of memory */
#define  EACCES          13  /* Permission denied */
#define  EFAULT          14  /* Bad address */
#define  ENOTBLK         15  /* Block device required */
#define  EBUSY           16  /* Device or resource busy */
#define  EEXIST          17  /* File exists */
#define  EXDEV           18  /* Cross-device link */
#define  ENODEV          19  /* No such device */
#define  ENOTDIR         20  /* Not a directory */
#define  EISDIR          21  /* Is a directory */
#define  EINVAL          22  /* Invalid argument */
#define  ENFILE          23  /* File table overflow */
#define  EMFILE          24  /* Too many open files */
#define  ENOTTY          25  /* Not a typewriter */
#define  ETXTBSY         26  /* Text file busy */
#define  EFBIG           27  /* File too large */
#define  ENOSPC          28  /* No space left on device */
#define  ESPIPE          29  /* Illegal seek */
#define  EROFS           30  /* Read-only file system */
#define  EMLINK          31  /* Too many links */
#define  EPIPE           32  /* Broken pipe */
#define  EDOM            33  /* Math argument out of domain of func */
#define  ERANGE          34  /* Math result not representable */
#ifndef EDEADLK
#define  EDEADLK         35  /* Resource deadlock would occur */
#endif
#ifndef ENAMETOOLONG
#define  ENAMETOOLONG    36  /* File name too long */
#endif
#ifndef ENOLCK
#define  ENOLCK          37  /* No record locks available */
#endif
#ifndef ENOSYS
#define  ENOSYS          38  /* Function not implemented */
#endif
#ifndef ENOTEMPTY
#define  ENOTEMPTY       39  /* Directory not empty */
#endif
#ifndef ELOOP
#define  ELOOP           40  /* Too many symbolic links encountered */
#endif
#define  EWOULDBLOCK     EAGAIN  /* Operation would block */
#ifndef ENOMSG
#define  ENOMSG          42  /* No message of desired type */
#endif
#ifndef EIDRM
#define  EIDRM           43  /* Identifier removed */
#endif
#define  ECHRNG          44  /* Channel number out of range */
#define  EL2NSYNC        45  /* Level 2 not synchronized */
#define  EL3HLT          46  /* Level 3 halted */
#define  EL3RST          47  /* Level 3 reset */
#define  ELNRNG          48  /* Link number out of range */
#define  EUNATCH         49  /* Protocol driver not attached */
#define  ENOCSI          50  /* No CSI structure available */
#define  EL2HLT          51  /* Level 2 halted */
#define  EBADE           52  /* Invalid exchange */
#define  EBADR           53  /* Invalid request descriptor */
#define  EXFULL          54  /* Exchange full */
#define  ENOANO          55  /* No anode */
#define  EBADRQC         56  /* Invalid request code */
#define  EBADSLT         57  /* Invalid slot */

#define  EDEADLOCK       EDEADLK

#define  EBFONT          59  /* Bad font file format */
#define  ENOSTR          60  /* Device not a stream */
#define  ENODATA         61  /* No data available */
#define  ETIME           62  /* Timer expired */
#define  ENOSR           63  /* Out of streams resources */
#define  ENONET          64  /* Machine is not on the network */
#define  ENOPKG          65  /* Package not installed */
#define  EREMOTE         66  /* Object is remote */
#define  ENOLINK         67  /* Link has been severed */
#define  EADV            68  /* Advertise error */
#define  ESRMNT          69  /* Srmount error */
#define  ECOMM           70  /* Communication error on send */
#define  EPROTO          71  /* Protocol error */
#ifndef EMULTIHOP
#define  EMULTIHOP       72  /* Multihop attempted */
#endif
#define  EDOTDOT         73  /* RFS specific error */
#ifndef EBADMSG
#define  EBADMSG         74  /* Not a data message */
#endif
#ifndef EOVERFLOW
#define  EOVERFLOW       75  /* Value too large for defined data type */
#endif
#define  ENOTUNIQ        76  /* Name not unique on network */
#define  EBADFD          77  /* File descriptor in bad state */
#define  EREMCHG         78  /* Remote address changed */
#define  ELIBACC         79  /* Can not access a needed shared library */
#define  ELIBBAD         80  /* Accessing a corrupted shared library */
#define  ELIBSCN         81  /* .lib section in a.out corrupted */
#define  ELIBMAX         82  /* Attempting to link in too many shared libraries */
#define  ELIBEXEC        83  /* Cannot exec a shared library directly */
#ifndef EILSEQ
#define  EILSEQ          84  /* Illegal byte sequence */
#endif
#define  ERESTART        85  /* Interrupted system call should be restarted */
#define  ESTRPIPE        86  /* Streams pipe error */
#define  EUSERS          87  /* Too many users */
#ifndef ENOTSOCK
#define  ENOTSOCK        88  /* Socket operation on non-socket */
#endif
#ifndef EDESTADDRREQ
#define  EDESTADDRREQ    89  /* Destination address required */
#endif
#ifndef EMSGSIZE
#define  EMSGSIZE        90  /* Message too long */
#endif
#ifndef EPROTOTYPE
#define  EPROTOTYPE      91  /* Protocol wrong type for socket */
#endif
#ifndef ENOPROTOOPT
#define  ENOPROTOOPT     92  /* Protocol not available */
#endif
#ifndef EPROTONOSUPPORT
#define  EPROTONOSUPPORT 93  /* Protocol not supported */
#endif
#define  ESOCKTNOSUPPORT 94  /* Socket type not supported */
#define  EOPNOTSUPP      95  /* Operation not supported on transport endpoint */
#define  EPFNOSUPPORT    96  /* Protocol family not supported */
#ifndef EAFNOSUPPORT
#define  EAFNOSUPPORT    97  /* Address family not supported by protocol */
#endif
#ifndef EADDRINUSE
#define  EADDRINUSE      98  /* Address already in use */
#endif
#ifndef EADDRNOTAVAIL
#define  EADDRNOTAVAIL   99  /* Cannot assign requested address */
#endif
#ifndef ENETDOWN
#define  ENETDOWN       100  /* Network is down */
#endif
#ifndef ENETUNREACH
#define  ENETUNREACH    101  /* Network is unreachable */
#endif
#ifndef ENETRESET
#define  ENETRESET      102  /* Network dropped connection because of reset */
#endif
#ifndef ECONNABORTED
#define  ECONNABORTED   103  /* Software caused connection abort */
#endif
#define  ECONNRESET     104  /* Connection reset by peer */
#define  ENOBUFS        105  /* No buffer space available */
#ifndef EISCONN
#define  EISCONN        106  /* Transport endpoint is already connected */
#endif
#ifndef ENOTCONN
#define  ENOTCONN       107  /* Transport endpoint is not connected */
#endif
#define  ESHUTDOWN      108  /* Cannot send after transport endpoint shutdown */
#ifndef ETOOMANYREFS
#define  ETOOMANYREFS   109  /* Too many references: cannot splice */
#endif
#ifndef ETIMEDOUT
#define  ETIMEDOUT      110  /* Connection timed out */
#endif
#define  ECONNREFUSED   111  /* Connection refused */
#ifndef EHOSTDOWN
#define  EHOSTDOWN      112  /* Host is down */
#endif
#ifndef EHOSTUNREACH
#define  EHOSTUNREACH   113  /* No route to host */
#endif
#ifndef EALREADY
#define  EALREADY       114  /* Operation already in progress */
#endif
#ifndef EINPROGRESS
#define  EINPROGRESS    115  /* Operation now in progress */
#endif
#ifndef ESTALE
#define  ESTALE         116  /* Stale NFS file handle */
#endif
#define  EUCLEAN        117  /* Structure needs cleaning */
#define  ENOTNAM        118  /* Not a XENIX named type file */
#define  ENAVAIL        119  /* No XENIX semaphores available */
#define  EISNAM         120  /* Is a named type file */
#define  EREMOTEIO      121  /* Remote I/O error */
#ifndef EDQUOT
#define  EDQUOT         122  /* Quota exceeded */
#endif

#define  ENOMEDIUM      123  /* No medium found */
#define  EMEDIUMTYPE    124  /* Wrong medium type */

#ifndef errno
extern int errno;
#endif

#else /* LWIP_PROVIDE_ERRNO */

/* Define LWIP_ERRNO_STDINCLUDE if you want to include <errno.h> here */
#ifdef LWIP_ERRNO_STDINCLUDE
#include <errno.h>
#else /* LWIP_ERRNO_STDINCLUDE */
/* Define LWIP_ERRNO_INCLUDE to an equivalent of <errno.h> to include the error defines here */
#ifdef LWIP_ERRNO_INCLUDE
#include LWIP_ERRNO_INCLUDE
#endif /* LWIP_ERRNO_INCLUDE */
#endif /* LWIP_ERRNO_STDINCLUDE */

#endif /* LWIP_PROVIDE_ERRNO */

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_ERRNO_H */
