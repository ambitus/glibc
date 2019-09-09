/* Dummy epoll_wait syscall implementation.
   Copyright (C) 2018 Rocket Software
   This file is part of the GNU C Library.
   Contributed by Giancarlo Frix <gfrix@rocketsoftware.com>, 2018.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library.  If not, see
   <http://www.gnu.org/licenses/>.

   TODO: This is a dummy.  */

#include <stddef.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <errno.h>

int
epoll_wait (int epfd, struct epoll_event *events, int maxevents, int timeout)
{
  return ENOSYS;
}
stub_warning (epoll_wait)
