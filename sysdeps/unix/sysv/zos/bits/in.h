/* Copyright (C) 2019 Free Software Foundation, Inc.
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

#ifndef _NETINET_IN_H
# error "Never use <bits/in.h> directly; include <netinet/in.h> instead."
#endif

#define __USE_KERNEL_IPV6_DEFS 0

/* To select the IP level.  */
#define SOL_IP		0

/* Options for use with `getsockopt' and `setsockopt' at the IP level.
   The first word in the comment at the right is the data type used;
   "bool" means a boolean value stored in an `int'.
   z/OS TODO: Add data types and descriptions.  */
/* z/OS TODO: Lowpri. We can emulate IP_HDRINCL, but should we?  */
/* z/OS TODO: Lowpri. What other IP_* opts can/should we emulate?  */
#define IP_TOS				2
#define IP_MULTICAST_TTL		3
#define IP_MULTICAST_LOOP		4
#define IP_ADD_MEMBERSHIP		5
#define IP_DROP_MEMBERSHIP		6
#define IP_MULTICAST_IF			7
#define IP_BLOCK_SOURCE			10
#define IP_UNBLOCK_SOURCE		11
#define IP_ADD_SOURCE_MEMBERSHIP	12
#define IP_DROP_SOURCE_MEMBERSHIP	13

#define IP_DEFAULT_MULTICAST_TTL	1
#define IP_DEFAULT_MULTICAST_LOOP	1
#define IP_MAX_MEMBERSHIPS		20

/* Protocol-independent options.  */
#ifdef __USE_MISC
# define MCAST_JOIN_GROUP 40	/* : join any-source group */
# define MCAST_BLOCK_SOURCE 44	/* : block from given group */
# define MCAST_UNBLOCK_SOURCE 45 /* : unblock from given group*/
# define MCAST_LEAVE_GROUP 41	/* : leave any-source group */
# define MCAST_JOIN_SOURCE_GROUP 42 /* : join source-spec gr */
# define MCAST_LEAVE_SOURCE_GROUP 43 /* : leave source-spec gr*/

# define MCAST_EXCLUDE   0
# define MCAST_INCLUDE   1
#endif

/* Socket-level values for IPv6.  */
#define SOL_IPV6		41
#define SOL_ICMPV6		58

/* Options for use with `getsockopt' and `setsockopt' at the IPv6 level.
   The first word in the comment at the right is the data type used;
   "bool" means a boolean value stored in an `int'.  */
/* z/OS TODO: lowpri. Are there any opts we should emulate? What about
   IPV6_2292PKTINFO/IPV6_RXINFO/IPV6_TXINFO/SCM_SRCINFO/SCM_SRCRT?  */
#define IPV6_UNICAST_HOPS	3
#define IPV6_MULTICAST_LOOP	4
#define IPV6_JOIN_GROUP		5
#define IPV6_LEAVE_GROUP	6
#define IPV6_MULTICAST_IF	7
#define IPV6_MULTICAST_HOPS	9
#define IPV6_V6ONLY		10
#define IPV6_HOPLIMIT		11
#define IPV6_PATHMTU		12
#define IPV6_PKTINFO		13
#define IPV6_RECVHOPLIMIT	14
#define IPV6_RECVPKTINFO	15
#define IPV6_RECVPATHMTU	16
#define IPV6_REACHCONF		17
#define IPV6_USE_MIN_MTU	18
#define IPV6_CHECKSUM		19
#define IPV6_NEXTHOP		20
#define IPV6_RTHDR		21
#define IPV6_HOPOPTS		22
#define IPV6_DSTOPTS		23
#define IPV6_RTHDRDSTOPTS	24
#define IPV6_RECVRTHDR		25
#define IPV6_RECVHOPOPTS	26
#define IPV6_RECVRTHDRDSOPTS	27
#define IPV6_RECVDSTOPTS	28
#define IPV6_DONTFRAG		29
#define IPV6_TCLASS		30
#define IPV6_RECVTCLASS		31
#define IPV6_ADDR_PREFERENCES	32

/* Obsolete synonyms for the above.  */
#define IPV6_ADD_MEMBERSHIP	IPV6_JOIN_GROUP
#define IPV6_DROP_MEMBERSHIP	IPV6_LEAVE_GROUP
#define IPV6_RXHOPOPTS		IPV6_HOPOPTS
#define IPV6_RXDSTOPTS		IPV6_DSTOPTS

/* Routing header options for IPv6.  */
#define IPV6_RTHDR_TYPE_0	0	/* IPv6 Routing header type 0.  */

#ifdef __USE_MISC
/* z/OS TODO: Define option-specific structures here that are not defined
   anywhere else.  */
#endif
