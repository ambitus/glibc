/* z/OS errnos.
   Copyright (C) 2019 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Giancarlo Frix <gfrix@rocketsoftware.com>.

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
   <http://www.gnu.org/licenses/>.

   TODO: Figure out what to do about missing errnos, and errnos that
   are aliased to other errnos on other platforms but are distinct on
   z/OS. Also just generally clean this file up.

   TODO: Determine how we can use errnojrs.

   TODO: Figure out what to do about sys_errlist. sys_errlist is a
   char * array designed to be indexed by errno and used to implement
   strerror(). It must be large enough to accommodate any value of
   errno. On most systems, errnos are assigned sequentially, so
   the maximal value of errno is generally in the 130s, and sys_errlist
   only takes up around 8 KB on 64-bit platforms. Since on z/OS errno
   is not continuous and the max value is 3448, a strictly conformant
   glibc implementation for us would have a sparse sys_errlist that
   takes up more than 220 KB!!! At the moment we capitulate to this
   requirement, however sys_errlist is deprecated, not required by
   POSIX, and as I'm not aware of many programs that use it, so we
   could probably exclude it from our build entirely. strerror will
   need to be rewritten. We wouldn't need to entirely remove
   sys_errlist, just define a function that maps from the current
   domain of errno to a non-sparse one. This somewhat mirrors the
   approch that the Hurd implementation takes. linux/MIPS also has
   a suspiciously large #errlist-compat number, so maybe check to
   see what's going on there.  */


#define EPERM 139
#define ENOENT 129
#define ESRCH 143
#define EINTR 120
#define EIO 122
#define ENXIO 138
#define E2BIG 145
#define ENOEXEC 130
#define EBADF 113
#define ECHILD 115
#define EAGAIN 112
#define ENOMEM 132
#define EACCES 111
#define EFAULT 118
#define ENOTBLK 1100
#define EBUSY 114
#define EEXIST 117
#define EXDEV 144
#define ENODEV 128
#define ENOTDIR 135
#define EISDIR 123
#define EINVAL 121
#define ENFILE 127
#define EMFILE 124
#define ENOTTY 137
#define ETXTBSY 1101
#define EFBIG 119
#define ENOSPC 133
#define ESPIPE 142
#define EROFS 141
#define EMLINK 125
#define EPIPE 140
#define EDOM 1
#define ERANGE 2


#define EDEADLK 116
#define ENAMETOOLONG 126
#define ENOLCK 131
#define ENOSYS 134
#define ENOTEMPTY 136
#define ELOOP 146
#define EWOULDBLOCK 1102 /* z/OS TODO: This is aliased to EAGAIN on other platforms. */
#define ENOMSG 1139
#define EIDRM 1141
/* z/OS TODO: missing ECHRNG */
/* z/OS TODO: missing EL2NSYNC */
/* z/OS TODO: missing EL3HLT */
/* z/OS TODO: missing EL3RST */
/* z/OS TODO: missing ELNRNG */
#define EUNATCH 3448
/* z/OS TODO: missing ENOCSI */
/* z/OS TODO: missing EL2HLT */
/* z/OS TODO: missing EBADE */
/* z/OS TODO: missing EBADR */
/* z/OS TODO: missing EXFULL */
/* z/OS TODO: missing ENOANO */
/* z/OS TODO: missing EBADRQC */
/* z/OS TODO: missing EBADSLT */

#define EDEADLOCK EDEADLK

/* z/OS TODO: missing EBFONT */
#define ENOSTR 1136
/* z/OS TODO: missing ENODATA */
#define ETIME 1137
#define ENOSR 1138
#define ENONET 1142
/* z/OS TODO: missing ENOPKG */
#define EREMOTE 1135 /* z/OS TODO: the description of ERREMOTE would seem
			to indicate that EREMOTE and ERREMOTE might be
			swapped, as in we should define EREMOTE to
			ERREMOTE and 1135 to something like
			VENDOR_EREMOTE. */
#define ENOLINK 1144
#define EADV 1145
#define ESRMNT 1146
#define ECOMM 1147
#define EPROTO 1148
#define EMULTIHOP 1149
#define EDOTDOT 1150
#define EBADMSG 1140
#define EOVERFLOW 149
/* z/OS TODO: missing ENOTUNIQ */
/* z/OS TODO: missing EBADFD */
#define EREMCHG 1151
/* z/OS TODO: missing ELIBACC */
/* z/OS TODO: missing ELIBBAD */
/* z/OS TODO: missing ELIBSCN */
/* z/OS TODO: missing ELIBMAX */
/* z/OS TODO: missing ELIBEXEC */
#define EILSEQ 147
/* z/OS TODO: missing ERESTART */
/* z/OS TODO: missing ESTRPIPE */
#define EUSERS 1132
#define ENOTSOCK 1105
#define EDESTADDRREQ 1106
#define EMSGSIZE 1107
#define EPROTOTYPE 1108
#define ENOPROTOOPT 1109
#define EPROTONOSUPPORT 1110
#define ESOCKTNOSUPPORT 1111
#define EOPNOTSUPP 1112
#define EPFNOSUPPORT 1113
#define EAFNOSUPPORT 1114
#define EADDRINUSE 1115
#define EADDRNOTAVAIL 1116
#define ENETDOWN 1117
#define ENETUNREACH 1118
#define ENETRESET 1119
#define ECONNABORTED 1120
#define ECONNRESET 1121
#define ENOBUFS 1122
#define EISCONN 1123
#define ENOTCONN 1124
#define ESHUTDOWN 1125
#define ETOOMANYREFS 1126
#define ETIMEDOUT 1127
#define ECONNREFUSED 1128
#define EHOSTDOWN 1129
#define EHOSTUNREACH 1130
#define EALREADY 1104
#define EINPROGRESS 1103
#define ESTALE 1134
/* z/OS TODO: missing EUCLEAN */
/* z/OS TODO: missing ENOTNAM */
/* z/OS TODO: missing ENAVAIL */
/* z/OS TODO: missing EISNAM */
/* z/OS TODO: missing EREMOTEIO */
#define EDQUOT 1133

/* z/OS TODO: missing ENOMEDIUM */
/* z/OS TODO: missing EMEDIUMTYPE */
#define ECANCELED 1152
/* z/OS TODO: missing ENOKEY */
/* z/OS TODO: missing EKEYEXPIRED */
/* z/OS TODO: missing EKEYREVOKED */
/* z/OS TODO: missing EKEYREJECTED */

/* z/OS TODO: missing EOWNERDEAD */
/* z/OS TODO: missing ENOTRECOVERABLE */

/* z/OS TODO: missing ERFKILL */

/* z/OS TODO: missing EHWPOISON */

#define ENOTSUP 247 /* z/OS TODO: this is *sometimes* aliased to
		       EOPNOTSUPP on other platforms. */


/* Some errnos that are used internally by glibc have no z/OS
   equivalents. We must define these ourselves. We define them to
   arbitrary unused values.

   z/OS TODO: Make sure that these values are future proof.  */
#define EOWNERDEAD 3
#define ENOTRECOVERABLE 4


/* z/OS-specific errnos  */

#define EMVSNOTUP 150
#define EMVSDYNALC 151
#define EMVSCVAF 152
#define EMVSCATLG 153
#define EMVSINITIAL 156
#define EMVSERR 157
#define EMVSPARM 158
#define EMVSPFSFILE 159
#define EMVSPFSPERM 162
#define EMVSSAFEXTRERR 163
#define EMVSSAF2ERR 164
#define EMVSNORTL 167
#define EMVSEXPIRE 168
#define EMVSPASSWORD 169
#define EMVSWLMERROR 170
#define EMVSCPLERROR 171
#define EMVSARMERROR 172
#define EIBMSOCKOUTOFRANGE 1002
#define EIBMSOCKINUSE 1003
#define EOFFLOADboxERROR 1005
#define EOFFLOADboxRESTART 1006
#define EOFFLOADboxDOWN 1007
#define EIBMCONFLICT 1008
#define EIBMCANCELLED 1009
#define EIBMBADTCPNAME 1011
#define EPROCLIM 1131
#define ERREMOTE 1143 /* z/OS TODO: see EREMOTE above. */
#define ETcpOutOfState 1153
#define ETcpUnattach 1154
#define ETcpBadObj 1155
#define ETcpClosed 1156
#define ETcpLinked 1157
#define ETcpErr 1158
#define EINTRNODATA 1159
#define ENOREUSE 1160
#define ENOMOVE 1161
