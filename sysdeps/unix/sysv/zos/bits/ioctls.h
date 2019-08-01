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

#ifndef _SYS_IOCTL_H
# error "Never use <bits/ioctls.h> directly; include <sys/ioctl.h> instead."
#endif

/* z/OS TODO: Figure out what the ioctls actually do, and compare this
   list against the linux list. See if we can use these to implement
   some other features.  */

#define IOCC_TCI		5000
#define IOCC_TCC		5001
#define IOCC_TCS		5002
#define IOCC_TCR		5003
#define IOCC_TCG		5004
#define IOCC_TCCE		5006
#define SIOCMSDELRT		5007
#define SIOCMSADDRT		5008
#define SIOCMSSIFADDR		5009
#define SIOCMSSIFFLAGS		5010
#define SIOCMSSIFDSTADDR	5011
#define SIOCMSSIFBRDADDR	5012
#define SIOCMSSIFNETMASK	5013
#define SIOCMSSIFMETRIC		5014
#define SIOCMSRBRTTABLE		5015
#define SIOMSMETRIC1RT		5016
#define SIOCMSICMPREDIRECT	5017
#define SIOCSETTKN		0x8008139A
#define SIOCMSADDRT6		0x8044F604
#define SIOCMSDELRT6		0x8044F605
#define SIOCGRT6TABLE		0xC014F606
#define SIOCGRT6TABLE64		0xC018F606
#define SIOCMSRBRT6TABLE	0x8000F607
#define SIOCGHOMEIF6		0xC014F608
#define SIOCGHOMEIF664		0xC018F608
#define SIOCMSRBHOMEIF6		0x8000F609
#define SIOCMSCHGRT6METRIC	0x8044F60A
#define SIOCMSMODHOMEIF6	0x8008F60B
#define SIOCMSADDRT6V2		0x8058F60C
#define SIOCMSDELRT6V2		0x8058F60D
#define SIOCMSCHGRT6METRICV2	0x8058F60E
#define SIOCGPARTNERINFO	0xC000F612
#define SIOCSPARTNERINFO	0x8004F613
#define TIOCGWINSZ		0x4008A368
#define TIOCSWINSZ		0x8008A367
#define TIOCNOTIFY		0x8001A364
#define SIOCGRTTABLE		0xC008C980
#define SIOCGRTTABLE64		0xC00CC980
#define SIOCSETRTTD		0x8008C981
#define SIOCMSMODHOMEIF		0x8008C983
#define SIOCMSMODHOMEIFV2	0x8016C984
#define SIOCMSADDRTV2		0x8054C985
#define SIOCMSDELRTV2		0x8054C986
#define SIOMSMETRIC1RTV2	0x8054C987
#define FIONBIO			0x8004A77E
#define FIONREAD		0x4004A77F
#define FIONWRITE		0x4004A78A
#define FIOASYNC		0x8004A77D
#define FIOSETOWN		0x8004A77C
#define FIOGETOWN		0x4004A77B
#define SECIGET			0x4010E401
#define SECIGET_T		0x4028E403
#define SIOCTIEDESTHRD		0x8004E404
#define SIOCSECENVR		0xC012A78B
#define SIOCADDRT		0x8030A70A
#define SIOCATMARK		0x4004A707
#define SIOCSPGRP		0x8004A708
#define SIOCGPGRP		0x4004A709
#define SIOCDELRT		0x8030A70B
#define SIOMETRIC1RT		0x8030A70C
#define SIOCSIFADDR		0x8020A70C
#define SIOCGIFADDR		0xC020A70D
#define SIOCGIFBRDADDR		0xC020A712
#define SIOCSIFBRDADDR		0x8020A713
#define SIOCGIFCONF		0xC008A714
#define SIOCGIFCONF64		0xC00CA714
#define SIOCGIFCONF6		0xC018A722
#define SIOCGIFMTU		0xC020A726
#define SIOCGIFDSTADDR		0xC020A70F
#define SIOCGIFFLAGS		0xC020A711
#define SIOCGIFMETRIC		0xC020A717
#define SIOCGIFNETMASK		0xC020A715
#define SIOCSIFNETMASK		0x8020A716
#define SIOCSIFDSTADDR		0x8020A70E
#define SIOCSIFFLAGS		0x8020A710
#define SIOCSIFMETRIC		0x8020A718
#define SIOCSARP		0x8024A71E
#define SIOCGARP		0xC024A71F
#define SIOCDARP		0x8024A720
#define SIOCSHIWAT		0x8004A700
#define SIOCGHIWAT		0x4004A701
#define SIOCSLOWAT		0x8004A702
#define SIOCGLOWAT		0x4004A703
#define FIOFCTLNBIO		0x0000E402
#define IOCC_ILINK		0x4004E21A
#define IOCC_EDITACL		0x2000C100
#define SETFACL			0x0000D301
#define GETFACL			0x0000D302
#define SIOCGSOCKPOEATTRS	0x4000D305
#define SIOCGFDPOEATTRS		0x4000D306
#define SIOCGSOCKMLSINFO	0x4000D307
