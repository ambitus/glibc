/* ESTAEX registration routines and wrappers.
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

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <sysdep.h>
#include <zos-core.h>
#include <zos-estaex.h>

int
__set_estaex_handler (void (*user_handler) (struct sdwa *, void *),
		      void *user_data)
{
  void *storage, *parm_list;
  struct estaex_handler_data *handler_data;
  uintptr_t aligned1, alloc_size;
  int return_code, reason_code;

  /* Calculate aligned storage requirements.  */
  alloc_size = aligned1 = (sizeof (*handler_data) + 7) & ~7;
  alloc_size += SIZEOF_ESTAEX_PARM_LIST;
  /* Align again because storage obtain only handles multiples of 8.  */
  alloc_size = (alloc_size + 7) & ~7;

  /* Acquire some below-the-bar storage. It will live for as long
     as the program does.  */
  storage = __storage_obtain_simple (alloc_size);

  /* These must be below the bar */
  handler_data = storage;
  parm_list = (void *) ((uintptr_t)storage + aligned1);

  handler_data->user_handler = user_handler;
  handler_data->user_data = user_data;

  __asm__ __volatile__ ("lgr  %%r1, %4\n\t"
			"st   %2, 16(%%r1)\n\t"
			"ni   0(%%r1), 223\n\t"
			"oi   0(%%r1), 4\n\t"
			"stg  %3, 8(%%r1)\n\t"
			"mvi  3(%%r1), 1\n\t"
			"la   %%r0, 0\n\t"
			"llgt %%r14, 16\n\t"
			"l    %%r14, 772(%%r14)\n\t"
			"l    %%r14, 176(%%r14)\n\t"
			"pc   0(%%r14)\n\t"
			"lr   %0, %%r15\n\t"
			"lr   %1, %%r0\n\t"
			: "=r" (return_code), "=r" (reason_code)
			: "r" (&__estaex_handler_wrapper),
			  "r" (handler_data), "r" (parm_list)
			: "r0", "r1", "r14", "r15");

  return return_code;
}

/* The dumper.  */

static
int ext_printf(const char *format, ...) {
  char dst[1024];
  va_list arg;
  int err;

  va_start (arg, format);
  int len = vsnprintf(dst, sizeof(dst), format, arg);
  va_end (arg);

  INTERNAL_SYSCALL (write, err, 3, 1, dst, len);

  return len;
}

static int
ext_fprintf (int fd, const char *format, ...)
{
  char dst[1024];
  va_list arg;
  int err;

  va_start (arg, format);
  int len = vsnprintf (dst, sizeof (dst), format, arg);
  va_end (arg);

  INTERNAL_SYSCALL (write, err, 3, fd, dst, len);

  return len;
}

static char hexDigits[16] ={ '0', '1', '2', '3',
			     '4', '5', '6', '7',
			     '8', '9', 'A', 'B',
			     'C', 'D', 'E', 'F'};

static const unsigned char printable[256] = {
  0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e,
  0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e,
  0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e,
  0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e,
  0x20, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x3c, 0x28, 0x2b, 0x7c,
  0x26, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x21, 0x24, 0x2a, 0x29, 0x3b, 0x5e,
  0x2d, 0x2f, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2c, 0x25, 0x5f, 0x3e, 0x3f,
  0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x60, 0x3a, 0x23, 0x2e, 0x27, 0x3d, 0x22,
  0x2e, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e,
  0x2e, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e,
  0x2e, 0x7e, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x2e, 0x2e, 0x2e, 0x5b, 0x2e, 0x2e,
  0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x5d, 0x2e, 0x2e,
  0x7b, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e,
  0x7d, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50, 0x51, 0x52, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e,
  0x5c, 0x2e, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e
};



int hexFill(char *buffer, int offset, int prePad, int formatWidth, int postPad, int value){
  int i;

  for (i=0; i<prePad; i++){
    buffer[offset+i] = ' ';
  }
  offset += prePad;

  for (i=0; i<formatWidth; i++){
    int shift = 4 * (formatWidth - i - 1);
    buffer[offset+i] = hexDigits[(value>>shift)&0xf];
  }
  offset += formatWidth;
  for (i=0; i<postPad; i++){
    buffer[offset+i] = ' ';
  }
  return offset+postPad;
}


void dump_buffer(const char *buffer, int length)
{
  int index = 0;
  int last_index;
  char lineBuffer[256];

  if (length <= 0){
    return;
  }

  for (last_index = length-1; last_index>=0 && 0 == buffer[last_index]; last_index--){}
  if (last_index < 0)
    ext_printf("the buffer is empty at %x\n",buffer);

  while (index <= last_index){
    int pos;
    int linePos = 0;
    linePos = hexFill(lineBuffer,linePos,0,8,2,index);
    for (pos=0; pos<32; pos++){

      if (((index+pos)%4 == 0) && ((index+pos)%32 != 0)){
        if ((index+pos)%16 == 0){
          lineBuffer[linePos++] = ' ';
        }
        lineBuffer[linePos++] = ' ';
      }
      if ((index+pos)<length){
        linePos = hexFill(lineBuffer,linePos,0,2,0,(0xFF&buffer[index+pos])); /* sprintf(lineBuffer+linePos,"%0.2x",(int)(0xFF&buffer[index+pos])); */
      } else {
        linePos += snprintf(lineBuffer+linePos,256,"  ");
      }
    }
    linePos += snprintf(lineBuffer+linePos,256," |");
    for (pos=0; pos<32 && (index+pos)<length; pos++){
      int ch = printable[0xFF & buffer[index+pos]];
      lineBuffer[linePos++] = ch;
    }
    lineBuffer[linePos++] = '|';
    lineBuffer[linePos++] = 0;
    ext_printf("%s\n",lineBuffer);
    index += 32;
  }
}

void
__zos_dump_stack (int fd, void *r13)
{
  void *first_ptr, *ptr, *last_valid_ptr;
  int frames = 0;
  char eyecat_true[] = { 0xc6, 0xf4, 0xe2, 0xc1 };

  first_ptr = ptr = last_valid_ptr = r13;

  ext_fprintf (fd, "Stack frames:\n");

  while (ptr && frames < 50)
    {
      char *eyecat = ((char *)ptr) + 4;
      ext_fprintf (fd, "Frame at %p:\n", ptr);

      if (memcmp (eyecat, eyecat_true, 4))
       {
         ext_fprintf (fd, "  NO EYECATCHER\n");
         break;
       }

      last_valid_ptr = ptr;
      for (int i = 0; i < 16; i += 2)
       ext_fprintf (fd, "  %2d: 0x%016x    %2d: 0x%016x\n",
                    (i + 14) % 16, *(((unsigned long *) ptr) + i + 1),
                    (i + 15) % 16, *(((unsigned long *) ptr) + i + 2));
      ptr = (void*) *(((unsigned long *) ptr) + 16);
      frames++;
    }

  ext_fprintf (fd, "\nStack dump (at 0x%016x):\n", last_valid_ptr);
  dump_buffer (last_valid_ptr,
              ((char*) first_ptr - (char*) last_valid_ptr) + 256);
}

void __estaex_handler_dump(struct sdwa* sdwa_ptr, void *user_data) {
  ext_printf("*** ESTAEX TRIGGERED ***\n");

  ext_printf("\n");
  ext_printf("SDWA:\n");
  dump_buffer((char*) sdwa_ptr, sizeof(struct sdwa));

  struct sdwaptrs *sdwaptrs = (struct sdwaptrs *) GET_PTR31_SAFE(&sdwa_ptr->sdwaptrs);
  ext_printf("\n");
  ext_printf("SDWAPTRS:\n");
  dump_buffer((char*) sdwaptrs, sizeof(struct sdwaptrs));

  struct sdwarc1 *sdwarc1 = (struct sdwarc1 *) GET_PTR31_SAFE(&sdwaptrs->sdwarc1);
  ext_printf("\n");
  ext_printf("SDWARC1:\n");
  dump_buffer((char*) sdwarc1, sizeof(struct sdwarc1));

  ext_printf("\n");
  ext_printf("SDWARC2:\n");
  dump_buffer((char*) GET_PTR31_SAFE(&sdwaptrs->sdwarc2), sizeof(struct sdwarc2));

  ext_printf("\n");
  ext_printf("SDWARC3:\n");
  dump_buffer((char*) GET_PTR31_SAFE(&sdwaptrs->sdwarc3), sizeof(struct sdwarc3));

  struct sdwarc4 *sdwarc4 = (struct sdwarc4 *) GET_PTR31_SAFE(&sdwaptrs->sdwarc4);
  ext_printf("\n");
  ext_printf("SDWARC4:\n");
  dump_buffer((char*) sdwarc4, sizeof(struct sdwarc4));

  ext_printf("\n");
  ext_printf("SDWARC5:\n");
  dump_buffer((char*) GET_PTR31_SAFE(&sdwaptrs->sdwarc5), sizeof(struct sdwarc5));

  ext_printf("\n");
  ext_printf("SDWANRC1:\n");
  dump_buffer((char*) GET_PTR31_SAFE(&sdwaptrs->sdwanrc1), sizeof(struct sdwanrc1));

  ext_printf("\n");
  ext_printf("SDWANRC2:\n");
  dump_buffer((char*) GET_PTR31_SAFE(&sdwaptrs->sdwanrc2), sizeof(struct sdwanrc2));

  ext_printf("\n");
  ext_printf("SDWANRC3:\n");
  dump_buffer((char*) GET_PTR31_SAFE(&sdwaptrs->sdwanrc3), sizeof(struct sdwanrc3));


  ext_printf("\n");
  ext_printf("GPRs:\n");
  for (int i = 0; i < 16; i += 2)
    ext_printf("  %2d: 0x%016x    %2d: 0x%016x\n", i, sdwarc4->gprs[i], i + 1, sdwarc4->gprs[i + 1]);

  ext_printf("\n");
  ext_printf("ARs:\n");
  for (int i = 0; i < 16; i += 2)
    ext_printf("  %2d: 0x%016x    %2d: 0x%016x\n", i, sdwarc1->ars[i], i + 1, sdwarc1->ars[i + 1]);

  ext_printf("\n");

  __zos_dump_stack (1, (void *) (uintptr_t) sdwarc4->gprs[13]);

  ext_printf("\n");
  ext_printf("*** END ESTAEX DUMP ***");
  ext_printf("\n");
}
