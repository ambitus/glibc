/* termios type and macro definitions.  z/OS version.
   Copyright (C) 1993-2018 Free Software Foundation, Inc.
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

#ifndef _TERMIOS_H
# error "Never include <bits/termios.h> directly; use <termios.h> instead."
#endif

typedef unsigned char	cc_t;
typedef unsigned int	tcflag_t;

/* z/OS TODO: This should really be char, but is that okay?  */
typedef unsigned int	speed_t;

#define NCCS 11
struct termios
{
  tcflag_t c_cflag;		/* control mode flags.
				   The first two bytes are ospeed and
				   ispeed, respectively.  */
  tcflag_t c_iflag;		/* input mode flags */
  tcflag_t c_lflag;		/* local mode flags */
  tcflag_t c_oflag;		/* output mode flags */
  cc_t c_cc[NCCS];		/* control characters */

  /* z/OS TODO: Should we artificially add c_line, c_ispeed, and
     c_ospeed? First off, ask the maintainers why they added c_ispeed
     and c_ospeed in the first place. How did it help anything? They
     are just discarded by tcsetattr.  */
};

/* c_cc characters */
#define VINTR 0
#define VQUIT 1
#define VERASE 2
#define VKILL 3
#define VEOF 4
#define VEOL 5
#define VMIN 6
#define VSTART 7
#define VSTOP 8
#define VSUSP 9
#define VTIME 10
/* z/OS TODO: No VSWTC, VREPRINT, VDISCARD, VWERASE, VLNEXT, VEOL2.  */

/* c_iflag bits */
#define BRKINT	0000001
#define ICRNL	0000002
#define IGNBRK	0000004
#define IGNCR	0000010
#define IGNPAR	0000020
#define INLCR	0000040
#define INPCK	0000100
#define ISTRIP	0000200
#define IXOFF	0000400
#define IXON	0001000
#define PARMRK	0002000
#define IUCLC	0004000
#define IXANY	0010000
/* z/OS TODO: No IMAXBEL, IUTF8.  */

/* c_oflag bits */
#define OPOST	0000001
#define OLCUC	0000002
#define ONLCR	0000004
#define OCRNL	0000010
#define ONOCR	0000020
#define ONLRET	0000040
#define OFILL	0000100
#define OFDEL	0000200
#if defined __USE_MISC || defined __USE_XOPEN
# define NLDLY	0000400
# define   NL0	0000000
# define   NL1	0000400
# define TABDLY	0006000
# define   TAB0	0000000
# define   TAB1	0002000
# define   TAB2	0004000
# define   TAB3	0006000
# define CRDLY	0030000
# define   CR0	0000000
# define   CR1	0010000
# define   CR2	0020000
# define   CR3	0030000
# define FFDLY	0040000
# define   FF0	0000000
# define   FF1	0040000
# define BSDLY	0100000
# define   BS0	0000000
# define   BS1	0100000
#endif

#define VTDLY	0200000
#define   VT0	0000000
#define   VT1	0200000

#ifdef __USE_MISC
# define XTABS	TABDLY
#endif

/* c_cflag bit meaning */
#ifdef __USE_MISC
# define CBAUD	037700000000
#endif
/* z/OS TODO: We have the shift included so that CBAUD & c_cflag will
   work as expected. Could this approach break anything?  */
#define	 B0	(0 << 24)		/* hang up */
#define	 B50	(1 << 24)
#define	 B75	(2 << 24)
#define	 B110	(3 << 24)
#define	 B134	(4 << 24)
#define	 B150	(5 << 24)
#define	 B200	(6 << 24)
#define	 B300	(7 << 24)
#define	 B600	(8 << 24)
#define	 B1200	(9 << 24)
#define	 B1800	(10 << 24)
#define	 B2400	(11 << 24)
#define	 B4800	(12 << 24)
#define	 B9600	(13 << 24)
#define	 B19200	(14 << 24)
#define	 B38400	(15 << 24)
#ifdef __USE_MISC
# define EXTA B19200
# define EXTB B38400
#endif
#define CLOCAL	0000001
#define CREAD	0000002
#define CSIZE	0000060
#define   CS5	0000000
#define   CS6	0000020
#define   CS7	0000040
#define   CS8	0000060
#define CSTOPB	0000200
#define HUPCL	0000400
#define PARENB	0001000
#define PARODD	0002000
/* z/OS TODO: The following are nonstandard, should we guard them with
   __USE_GNU? They could conflict with stuff.  */
#define CPKTMOD	0004000		/* Called PACKET by the docs, but that
				   name could potentially conflict with
				   many names, so we reanamed it.  */
#define PKT3270 0010000
#define PTU3270 0020000
#define PKTXTND 0020000
#ifdef __USE_MISC
/* z/OS TODO: CBAUDEX makes no sense for us.  */
# define CBAUDEX 0000000
#endif
/* z/OS TODO: We apparently don't support B7200, B14400, B28800,
   or anything above B38400.  */
#define __MAX_BAUD B38400
#ifdef __USE_MISC
/* z/OS TODO: We don't support them, so we define them to zero right now.
   Would it be better to not define them at all?  */
# define CIBAUD	  000000000000		/* input baud rate (not used) */
# define CMSPAR   000000000000		/* mark or space (stick) parity */
# define CRTSCTS  000000000000		/* flow control */
#endif

/* c_lflag bits */
#define ECHONL	0000001
#define ECHOE	0000002
#define ECHOK	0000004
#define ECHO	0000010
#define ICANON	0000020
#define IEXTEN	0000040
#define ISIG	0000100
#if defined __USE_MISC || (defined __USE_XOPEN && !defined __USE_XOPEN2K)
# define XCASE	0000200
#endif
#define TOSTOP	000020000000
#define NOFLSH	020000000000

#ifdef __USE_MISC
/* z/OS TODO: No ECHOCTL, ECHOPRT, ECHOKE, FLUSHO, PENDIN, EXTPROC.  */
#endif

/* tcflow() and TCXONC use these */
#define	TCOOFF		0
#define	TCOON		1
#define	TCIOFF		2
#define	TCION		3

/* tcflush() and TCFLSH use these */
#define	TCIFLUSH	0
#define	TCOFLUSH	1
#define	TCIOFLUSH	2

/* tcsetattr uses these */
#define	TCSANOW		0
#define	TCSADRAIN	1
#define	TCSAFLUSH	2


/* z/OS TODO: Fix _IOT_termios.  */
#define _IOT_termios /* Hurd ioctl type field.  */			\
  _IOT (_IOTS (cflag_t), 4, _IOTS (cc_t), NCCS, _IOTS (speed_t), 2)
