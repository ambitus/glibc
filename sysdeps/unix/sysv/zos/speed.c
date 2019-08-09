/* `struct termios' speed frobnication functions.  z/OS version.
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
   <http://www.gnu.org/licenses/>.  */

#include <stddef.h>
#include <errno.h>
#include <termios.h>

/* Return the output baud rate stored in *TERMIOS_P.  */
speed_t
cfgetospeed (const struct termios *termios_p)
{
  return termios_p->c_cflag & CBAUD;
}

/* Return the input baud rate stored in *TERMIOS_P.
   The numerical 0 is a special case for the input baud rate. It
   should set the input baud rate to the output baud rate.
   z/OS TODO: IMPORTANT: Is this something we need to manually
   set? When? Would need to be in tcsetattr, but is 0 a valid value
   in the kernel in its own right?  */
speed_t
cfgetispeed (const struct termios *termios_p)
{
  return (termios_p->c_cflag & 0x00ff0000) << 8;
}

/* Set the output baud rate stored in *TERMIOS_P to SPEED.  */
int
cfsetospeed (struct termios *termios_p, speed_t speed)
{
  if ((speed & ~CBAUD) != 0
      || (speed < B0 || speed > __MAX_BAUD))
    return INLINE_SYSCALL_ERROR_RETURN_VALUE (EINVAL);

  termios_p->c_cflag &= ~CBAUD;
  termios_p->c_cflag |= speed;

  return 0;
}
libc_hidden_def (cfsetospeed)


/* Set the input baud rate stored in *TERMIOS_P to SPEED.
   The numerical 0 is a special case for the input baud rate. It
   should set the input baud rate to the output baud rate.
   z/OS TODO: IMPORTANT: Is this something we need to manually
   set? When? Would need to be in tcsetattr, but is 0 a valid value
   in the kernel in its own right?  */
int
cfsetispeed (struct termios *termios_p, speed_t speed)
{
  if ((speed & ~CBAUD) != 0
      || (speed < B0 || speed > __MAX_BAUD))
    return INLINE_SYSCALL_ERROR_RETURN_VALUE (EINVAL);

  termios_p->c_cflag &= ~(CBAUD >> 8);
  termios_p->c_cflag |= speed >> 8;

  return 0;
}
libc_hidden_def (cfsetispeed)
