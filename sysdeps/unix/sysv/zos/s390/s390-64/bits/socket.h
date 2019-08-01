/* System-specific socket constants and types.  z/OS version.
   Copyright (C) 2019 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#ifndef __BITS_SOCKET_H
#define __BITS_SOCKET_H

#ifndef _SYS_SOCKET_H
# error "Never include <bits/socket.h> directly; use <sys/socket.h> instead."
#endif

#define	__need_size_t
#include <stddef.h>

#include <sys/types.h>

/* Type for length arguments in socket calls.  */
#ifndef __socklen_t_defined
typedef __socklen_t socklen_t;
# define __socklen_t_defined
#endif

/* Get the architecture-dependent definition of enum __socket_type.  */
#include <bits/socket_type.h>

/* Protocol families.  */
#define PF_UNSPEC	0	/* Unspecified.  */
#define PF_LOCAL	1	/* Local to host (pipes and file-domain).  */
#define PF_UNIX		PF_LOCAL /* POSIX name for PF_LOCAL.  */
#define PF_FILE		PF_LOCAL /* Another non-standard name for PF_LOCAL.  */
#define PF_INET		2	/* IP protocol family.  */
#define PF_IMPLINK	3	/* Arpanet IMP.  */
#define PF_PUP		4	/* The Pup protocols (BSP).  */
#define PF_CHAOS	5	/* MIT CHAOS.  */
#define PF_NS		6	/* XEROX NS.  */
#define PF_NBS		7	/* The NBS protocols.  */
#define PF_ECMA		8	/* ECMA.  */
#define PF_DATAKIT	9	/* Datakit.  */
#define PF_CCITT	10	/* CCITT X.25.  */
#define PF_SNA		11	/* IBM's Systems Network Arch.  */
#define PF_DECNET	12	/* DECNet.  */
#define PF_DLI		13	/* Direct Data Link Interface.  */
#define PF_LAT		14	/* Local Area Transport.  */
#define PF_HYLINK	15	/* NSC HYPERchannel.  */
#define PF_APPLETALK	16	/* Appletalk DDP.  */
#define PF_IUCV		17	/* IUCV sockets.  */
#define PF_ESCON	18	/* IBM's ESCON UDP.  */
#define PF_INET6	19	/* IP version 6.  */
#define PF_NETLINK	20
#define PF_ROUTE	PF_NETLINK /* Alias to emulate 4.4BSD.  */
#define PF_MAX		21	/* For now..  */

/* Address families.  */
#define AF_UNSPEC	PF_UNSPEC
#define AF_LOCAL	PF_LOCAL
#define AF_UNIX		PF_UNIX
#define AF_FILE		PF_FILE
#define AF_INET		PF_INET
#define AF_IMPLINK	PF_IMPLINK
#define AF_PUP		PF_PUP
#define AF_CHAOS	PF_CHAOS
#define AF_NS		PF_NS
#define AF_NBS		PF_NBS
#define AF_ECMA		PF_ECMA
#define AF_DATAKIT	PF_DATAKIT
#define AF_CCITT	PF_CCITT
#define AF_SNA		PF_SNA
#define AF_DECNET	PF_DECNET
#define AF_DLI		PF_DLI
#define AF_LAT		PF_LAT
#define AF_HYLINK	PF_HYLINK
#define AF_APPLETALK	PF_APPLETALK
#define AF_IUCV		PF_IUCV
#define AF_ESCON	PF_ESCON
#define AF_INET6	PF_INET6
#define AF_NETLINK	PF_NETLINK
#define AF_ROUTE	PF_ROUTE
#define AF_MAX		PF_MAX

/* Maximum queue length specifiable by listen.  */
#define SOMAXCONN	10	/* Default is ten, but the z/OS
				   communications server can be
				   configured to allow more.  */

/* Get the definition of the macro to define the common sockaddr members.  */
#include <bits/sockaddr.h>

/* Structure describing a generic socket address.
   z/OS TODO: IMPORTANT!!!! this isn't the sockaddr that the kernel uses,
   the kernel splits the family field into length (it depends of what), and
   family, which probably messes everything up. Figure out if we should expose
   that, or hide it with internal structure translation.  */
struct sockaddr
{
  __SOCKADDR_COMMON (sa_);	/* Common data: address family and length.  */
  char sa_data[14];		/* Address data.  */
};


/* Structure large enough to hold any socket address (with the historical
   exception of AF_UNIX).  */
#define __ss_aligntype	unsigned long int
#define _SS_PADSIZE						\
  (_SS_SIZE - __SOCKADDR_COMMON_SIZE - sizeof (__ss_aligntype))

struct sockaddr_storage
{
  __SOCKADDR_COMMON (ss_);	/* Address family, etc.  */
  char __ss_padding[_SS_PADSIZE];
  __ss_aligntype __ss_align;	/* Force desired alignment.  */
};


/* Bits in the FLAGS argument to `send', `recv', et al.  */
enum
  {
    MSG_OOB		= 0x01,	/* Process out-of-band data.  */
#define MSG_OOB		MSG_OOB
    MSG_PEEK		= 0x02,	/* Peek at incoming messages.  */
#define MSG_PEEK	MSG_PEEK
    MSG_DONTROUTE	= 0x04,	/* Don't use local routing.  */
#define MSG_DONTROUTE	MSG_DONTROUTE
#ifdef __USE_GNU
    /* DECnet uses a different name.  */
    MSG_TRYHARD		= MSG_DONTROUTE,
# define MSG_TRYHARD	MSG_DONTROUTE
#endif
    MSG_EOR		= 0x08, /* End of record.  */
#define MSG_EOR		MSG_EOR
    MSG_TRUNC		= 0x10,
#define MSG_TRUNC	MSG_TRUNC
    MSG_CTRUNC		= 0x20,	/* Control data lost before delivery.  */
#define MSG_CTRUNC	MSG_CTRUNC
    MSG_WAITALL		= 0x40, /* Wait for a full request.  */
#define MSG_WAITALL	MSG_WAITALL
    MSG_CONNTERM	= 0x80, /* Block until the TCP connection is
				   terminated. This is unique to z/OS,
				   and its use is nonportable.  */
#define MSG_CONNTERM	MSG_CONNTERM
    MSG_EOF		= 0x8000, /* Close socket once send() completes.
				     Nonportable. Acts like SCTP_EOF.  */
#define MSG_EOF		MSG_EOF
    /* z/OS TODO: See if we can emulate MSG_NOSIGNAL for relevant socket
       calls by setting THLINOSIG in the syscall wrapper.  */
    MSG_NOSIGNAL	= 0x010000	/* Do not generate SIGPIPE on EPIPE.
					   This flag is not interpreted by
					   the kernel, we emulate it.  */
#define MSG_NOSIGNAL MSG_NOSIGNAL

    /* z/OS TODO: Should we provide a define for MSG_FIN, maybe making it
       an alias of MSG_EOF? Would need to test what MSG_FIN does.  */

    /* We don't have MSG_CMSG_CLOEXEC, and can't effectively emulate
       it.  */
  };


/* Structure describing messages sent by `sendmsg' and received by
   `recvmsg'. This is different for AMODE 31 programs.  */
struct msghdr
  {
    void *msg_name;		/* Address to send to/receive from.  */
    struct iovec *msg_iov;	/* Vector of data to send/receive into.  */
    void *msg_control;		/* Ancillary data (eg BSD filedesc passing). */

    int msg_flags;		/* Flags on received message.  */
    socklen_t msg_namelen;	/* Length of address data.  */
    socklen_t msg_iovlen;	/* Number of elements in the vector.  */
    socklen_t msg_controllen;	/* Ancillary data buffer length.  */
  };

/* Structure used for storage of ancillary data object information.  */
struct cmsghdr
  {
    socklen_t cmsg_len;		/* Length of data in cmsg_data plus length
				   of cmsghdr structure.  */
    int cmsg_level;		/* Originating protocol.  */
    int cmsg_type;		/* Protocol specific type.  */
#if __glibc_c99_flexarr_available
    __extension__ unsigned char __cmsg_data __flexarr; /* Ancillary data.  */
#endif
  };

/* Ancillary data object manipulation macros.  */
#if __glibc_c99_flexarr_available
# define CMSG_DATA(cmsg) ((cmsg)->__cmsg_data)
#else
# define CMSG_DATA(cmsg) ((unsigned char *) ((struct cmsghdr *) (cmsg) + 1))
#endif
#define CMSG_NXTHDR(mhdr, cmsg) __cmsg_nxthdr (mhdr, cmsg)
#define CMSG_FIRSTHDR(mhdr) \
  ((size_t) (mhdr)->msg_controllen >= sizeof (struct cmsghdr)		      \
   ? (struct cmsghdr *) (mhdr)->msg_control : (struct cmsghdr *) 0)
#define CMSG_ALIGN(len) (((len) + sizeof (size_t) - 1) \
			 & (size_t) ~(sizeof (size_t) - 1))
#define CMSG_SPACE(len) (CMSG_ALIGN (len) \
			 + CMSG_ALIGN (sizeof (struct cmsghdr)))
#define CMSG_LEN(len)   (CMSG_ALIGN (sizeof (struct cmsghdr)) + (len))

extern struct cmsghdr *__cmsg_nxthdr (struct msghdr *__mhdr,
				      struct cmsghdr *__cmsg) __THROW;
#ifdef __USE_EXTERN_INLINES
# ifndef _EXTERN_INLINE
#  define _EXTERN_INLINE __extern_inline
# endif
_EXTERN_INLINE struct cmsghdr *
__NTH (__cmsg_nxthdr (struct msghdr *__mhdr, struct cmsghdr *__cmsg))
{
  if ((size_t) __cmsg->cmsg_len < sizeof (struct cmsghdr))
    /* The kernel header does this so there may be a reason.  */
    return (struct cmsghdr *) 0;

  __cmsg = (struct cmsghdr *) ((unsigned char *) __cmsg
			       + CMSG_ALIGN (__cmsg->cmsg_len));
  if ((unsigned char *) (__cmsg + 1) > ((unsigned char *) __mhdr->msg_control
					+ __mhdr->msg_controllen)
      || ((unsigned char *) __cmsg + CMSG_ALIGN (__cmsg->cmsg_len)
	  > ((unsigned char *) __mhdr->msg_control + __mhdr->msg_controllen)))
    /* No more entries.  */
    return (struct cmsghdr *) 0;
  return __cmsg;
}
#endif	/* Use `extern inline'.  */

/* Socket level message types.  */
enum
  {
    SCM_RIGHTS = 0x01		/* Transfer file descriptors.  */
#define SCM_RIGHTS SCM_RIGHTS
    /* z/OS TODO: should we guard with with __USE_GNU, like linux
       does?  */
    , SCM_SECINFO = 0x4002	/* z/OS security info passing.  */
#define SCM_SECINFO SCM_SECINFO
  };

/* Socket level values. We've only got the one.  */
#define	SOL_SOCKET	0xffff

/* Socket-level options for `getsockopt' and `setsockopt'.  */
enum
  {
   SO_DEBUG = 0x0001,		/* Record debugging information.  */
#define SO_DEBUG SO_DEBUG
   SO_ACCEPTCONN = 0x0002,	/* Accept connections on socket.  */
#define SO_ACCEPTCONN SO_ACCEPTCONN
   SO_REUSEADDR = 0x0004,	/* Allow reuse of local addresses.  */
#define SO_REUSEADDR SO_REUSEADDR
   SO_KEEPALIVE = 0x0008,	/* Keep connections alive and send
				   SIGPIPE when they die.  */
#define SO_KEEPALIVE SO_KEEPALIVE
   SO_DONTROUTE = 0x0010,	/* Don't do local routing.  */
#define SO_DONTROUTE SO_DONTROUTE
   SO_BROADCAST = 0x0020,	/* Allow transmission of
				   broadcast messages.  */
#define SO_BROADCAST SO_BROADCAST
   SO_USELOOPBACK = 0x0040,	/* Use the software loopback to avoid
				   hardware use when possible.  */
#define SO_USELOOPBACK SO_USELOOPBACK
   SO_LINGER = 0x0080,		/* Block on close of a reliable
				   socket to transmit pending data.  */
#define SO_LINGER SO_LINGER
   SO_OOBINLINE = 0x0100,	/* Receive out-of-band data in-band.  */
#define SO_OOBINLINE SO_OOBINLINE
   /* z/OS TODO: IMPORTANT!!! What is the difference between REUSEPORT
      and REUSEPORT2 (in the docs)? Which one provides the proper
      behavior for REUSEPORT?  */
   SO_REUSEPORT = 0x0200,	/* Allow local address and port reuse.  */
#define SO_REUSEPORT SO_REUSEPORT
   SO_SNDBUF = 0x1001,		/* Send buffer size.  */
#define SO_SNDBUF SO_SNDBUF
   SO_RCVBUF = 0x1002,		/* Receive buffer.  */
#define SO_RCVBUF SO_RCVBUF
   SO_SNDLOWAT = 0x1003,	/* Send low-water mark.  */
#define SO_SNDLOWAT SO_SNDLOWAT
   SO_RCVLOWAT = 0x1004,	/* Receive low-water mark.  */
#define SO_RCVLOWAT SO_RCVLOWAT
   SO_SNDTIMEO = 0x1005,	/* Send timeout.  */
#define SO_SNDTIMEO SO_SNDTIMEO
   SO_RCVTIMEO = 0x1006,	/* Receive timeout.  */
#define SO_RCVTIMEO SO_RCVTIMEO
   SO_ERROR = 0x1007,		/* Get and clear error status.  */
#define SO_ERROR SO_ERROR
   SO_STYLE = 0x1008,		/* Get socket connection style.  */
#define SO_STYLE SO_STYLE
   SO_TYPE = SO_STYLE		/* Compatible name for SO_STYLE.  */
#define SO_TYPE SO_TYPE
  };

/* Structure used to manipulate the SO_LINGER option.  */
struct linger
  {
    int l_onoff;		/* Nonzero to linger on close.  */
    int l_linger;		/* Time to linger.  */
  };

#endif	/* bits/socket.h */
