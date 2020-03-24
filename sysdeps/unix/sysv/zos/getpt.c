/* Copyright (C) 2020 Free Software Foundation, Inc.
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

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>

/* Prefix for master pseudo terminal nodes.  */
#define _PATH_PTY "/dev/ptyp"


/* Open a master pseudo terminal and return its file descriptor.
   POSIX-specific interface. */
int
__posix_openpt (int oflag)
{
  char buf[sizeof (_PATH_PTY) + 4];
  const char *c1, *c2, *c3, *c4;
  char *s;

  /* Digits to list names of master pseudo terminals. */
  const char ptyname[] = "0123456789";

  s = mempcpy (buf, _PATH_PTY, sizeof (_PATH_PTY) - 1);
  s[4] = '\0';

  /* Start at /dev/ptyp0000 and keep trying until we successfully
     open a master pseudo terminal or until we run out of devices. */
  for (c1 = ptyname; *c1 != '\0'; ++c1)
    {
      s[0] = *c1;
      for (c2 = ptyname; *c2 != '\0'; ++c2)
	{
	  s[1] = *c2;
	  for (c3 = ptyname; *c3 != '\0'; ++c3)
	    {
	      s[2] = *c3;
	      for (c4 = ptyname; *c4 != '\0'; ++c4)
		{
		  int fd;
		  s[3] = *c4;

		  /* Try to open the master. */
		  fd = open (buf, oflag);
		  if (fd != -1)
		    {
		      /* Master is opened successfully.
			 Clear HUPCL terminal attribute to be
			 able to reopen slave pseudo terminal. */
		      struct termios term;
		      if (tcgetattr(fd, &term) != 0)
			{
			  close(fd);
			  return -1;
			}
		      term.c_cflag &= ~((tcflag_t)HUPCL);
		      if (tcsetattr(fd, TCSANOW, &term) != 0)
			{
			  close(fd);
			  return -1;
			}

		      return fd;
		    }

		  if (errno == ENOENT)
		    /* Terminate the search, as all
		       pseudo terminals are in use. */
		    return -1;
		}
	    }
	}
    }

  /* All pseudo terminals are in use. */
  __set_errno (ENOENT);
  return -1;
}
weak_alias (__posix_openpt, posix_openpt)


/* Open a master pseudo terminal and return its file descriptor.
   glibc-specific interface. */
int
__getpt (void)
{
  return __posix_openpt (O_RDWR);
}
weak_alias (__getpt, getpt)
