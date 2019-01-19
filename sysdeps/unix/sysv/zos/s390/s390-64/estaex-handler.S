/* ESTAEX handler routine.
   Copyright (C) 2018 Rocket Software
   This file is part of the GNU C Library.
   Contributed by Michael Colavita <mcolavita@rocketsoftware.com>, 2018.

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

.text
  .align 8
.global estaex_handler
  .type estaex_handler, @function
estaex_handler:
  /*
   * When this routine is entered, registers are allocated as follows:
   * r0: equal to 12 if there is no SDWA
   * r1: SDWA
   * r2: Address of ESTAEX PARM parameter
   * r14: Return address (must be preserved)
   */
  bakr  %r14, %r0
  /* Validate presence of SDWA and save */
  chi   %r0, 12
  je    .Lestaex_abort
  lgr   %r3, %r1
  /* Validate handler data */
  ltgr  %r2, %r2
  jz    .Lestaex_abort
  /* Load and validate user handler */
  lg    %r4, 0(%r2)
  ltgr  %r4, %r4
  jz    .Lestaex_abort
  /* TODO: eyecatcher validation? PSW key work? */
  /* Save r14 */
  lgr   %r5, %r14
  /* Set up a stack and arguments */
  larl  %r1, estaex_stack
  stg   %r3, 0(%r1) /* Parameter 1: SDWA */
  lg    %r3, 8(%r2)
  stg   %r3, 8(%r1) /* Parameter 2: User data */
  llilh %r14, 50932
  oill  %r14, 58049
  st    %r14, 4(%r13)
  la    %r14, 16(%r1)
  stg   %r14, 136(%r13)
  /* Invoke the user handler */
  basr  %r14, %r4
  /* Restore r14 */
  lgr   %r14, %r5
.Lestaex_abort:
  /* End ESTAEX processing */
  eregg %r0, %r1
  pr

