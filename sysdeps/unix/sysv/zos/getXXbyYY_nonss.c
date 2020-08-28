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

#include <assert.h>
#include <errno.h>
#include <libc-lock.h>
#include <stdlib.h>

/*******************************************************************\
|* Here we assume several symbols to be defined:		   *|
|*								   *|
|* LOOKUP_TYPE   - the return type of the function		   *|
|*								   *|
|* FUNCTION_NAME - name of the non-reentrant function		   *|
|*								   *|
|* ADD_PARAMS    - additional parameter, can vary in number	   *|
|*								   *|
|* ADD_VARIABLES - names of additional parameter		   *|
|*								   *|
|* BUFLEN	 - length of buffer allocated for the non	   *|
|*		   reentrant version				   *|
|*								   *|
\*******************************************************************/

/* To make the real sources a bit prettier.  */
#define REENTRANT_NAME APPEND_R (FUNCTION_NAME)
#define APPEND_R(name) APPEND_R1 (name)
#define APPEND_R1(name) name##_r
#define INTERNAL(name) INTERNAL1 (name)
#define INTERNAL1(name) __##name

/* Prototype for reentrant version we use here.  */
extern int INTERNAL (REENTRANT_NAME) (ADD_PARAMS, LOOKUP_TYPE *resbuf,
				      char *buffer, size_t buflen,
				      LOOKUP_TYPE **result)
     attribute_hidden;

/* We need to protect the dynamic buffer handling.  */
__libc_lock_define_initialized (static, lock);

/* This points to the static buffer used.  */
libc_freeres_ptr (static char *buffer);


LOOKUP_TYPE *
FUNCTION_NAME (ADD_PARAMS)
{
  static size_t buffer_size;
  static LOOKUP_TYPE resbuf;
  LOOKUP_TYPE *result;

  /* Get lock.  */
  __libc_lock_lock (lock);

  if (buffer == NULL)
    {
      buffer_size = BUFLEN;
      buffer = (char *) malloc (buffer_size);
    }

  while (buffer != NULL
	 && (INTERNAL (REENTRANT_NAME) (ADD_VARIABLES, &resbuf, buffer,
					buffer_size, &result)
	     == ERANGE))
    {
      char *new_buf;
      buffer_size *= 2;
      new_buf = (char *) realloc (buffer, buffer_size);
      if (new_buf == NULL)
	{
	  /* We are out of memory.  Free the current buffer so that the
	     process gets a chance for a normal termination.  */
	  free (buffer);
	  __set_errno (ENOMEM);
	}
      buffer = new_buf;
    }

  if (buffer == NULL)
    result = NULL;

  /* Release lock.  */
  __libc_lock_unlock (lock);

  return result;
}
