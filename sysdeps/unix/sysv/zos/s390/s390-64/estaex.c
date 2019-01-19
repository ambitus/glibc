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

#include <stdio.h>
#include <stdarg.h>
#include <zos-estaex.h>

/* Modify with care. This struct is referenced in assembly. */
struct estaex_handler_data {
  void (*user_handler)(struct sdwa *, void *);
  void *user_data;
};

char estaex_stack[1 << 20];

extern void (*estaex_handler)(void);

int set_estaex_handler(void (*user_handler)(struct sdwa *, void *), void *user_data) {
  int return_code, reason_code;

  /* TODO: This must be below the bar */
  static char parm_list[24];

  /* TODO: storage obtain this */
  static struct estaex_handler_data data;
  data.user_handler = user_handler;
  data.user_data = user_data;

  __asm__ __volatile__(
      "lgr  %%r1, %4\n\t"
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
      : "r" (&estaex_handler), "r" (&data), "r" (parm_list)
      : "r0", "r1", "r14", "r15");

  return return_code;
}

/* The dumper.  */

#define EXTEND_PTR(x) ((void*) (x))
#define CVT_ADDR 0x10
#define CVT_CSRT_OFFSET 0x220
#define CVT ((char *)(uintptr_t)(*(int *) CVT_ADDR))
#define CVT_CSRT ((char*)(uintptr_t)(*(int *) (CVT + CVT_CSRT_OFFSET)))
#define CVT_CSRT_GET_LEVEL1(x) ((char *)(uintptr_t)(*(int *) (CVT_CSRT + x)))
#define CVT_CSRT_GET(x,y) ((void*) (uintptr_t)(*(int *) (CVT_CSRT_GET_LEVEL1(x) + y)))
static
const unsigned char iso88591_to_ibm1047[256] = {
/*         0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F */
/* 0 */ 0x00, 0x01, 0x02, 0x03, 0x37, 0x2D, 0x2E, 0x2F, 0x16, 0x05, 0x15, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
/* 1 */ 0x10, 0x11, 0x12, 0x13, 0x3C, 0x3D, 0x32, 0x26, 0x18, 0x19, 0x3F, 0x27, 0x1C, 0x1D, 0x1E, 0x1F,
/* 2 */ 0x40, 0x5A, 0x7F, 0x7B, 0x5B, 0x6C, 0x50, 0x7D, 0x4D, 0x5D, 0x5C, 0x4E, 0x6B, 0x60, 0x4B, 0x61,
/* 3 */ 0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0x7A, 0x5E, 0x4C, 0x7E, 0x6E, 0x6F,
/* 4 */ 0x7C, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6,
/* 5 */ 0xD7, 0xD8, 0xD9, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xAD, 0xE0, 0xBD, 0x5F, 0x6D,
/* 6 */ 0x79, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96,
/* 7 */ 0x97, 0x98, 0x99, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xC0, 0x4F, 0xD0, 0xA1, 0x07,
/* 8 */ 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x06, 0x17, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x09, 0x0A, 0x1B,
/* 9 */ 0x30, 0x31, 0x1A, 0x33, 0x34, 0x35, 0x36, 0x08, 0x38, 0x39, 0x3A, 0x3B, 0x04, 0x14, 0x3E, 0xFF,
/* A */ 0x41, 0xAA, 0x4A, 0xB1, 0x9F, 0xB2, 0x6A, 0xB5, 0xBB, 0xB4, 0x9A, 0x8A, 0xB0, 0xCA, 0xAF, 0xBC,
/* B */ 0x90, 0x8F, 0xEA, 0xFA, 0xBE, 0xA0, 0xB6, 0xB3, 0x9D, 0xDA, 0x9B, 0x8B, 0xB7, 0xB8, 0xB9, 0xAB,
/* C */ 0x64, 0x65, 0x62, 0x66, 0x63, 0x67, 0x9E, 0x68, 0x74, 0x71, 0x72, 0x73, 0x78, 0x75, 0x76, 0x77,
/* D */ 0xAC, 0x69, 0xED, 0xEE, 0xEB, 0xEF, 0xEC, 0xBF, 0x80, 0xFD, 0xFE, 0xFB, 0xFC, 0xBA, 0xAE, 0x59,
/* E */ 0x44, 0x45, 0x42, 0x46, 0x43, 0x47, 0x9C, 0x48, 0x54, 0x51, 0x52, 0x53, 0x58, 0x55, 0x56, 0x57,
/* F */ 0x8C, 0x49, 0xCD, 0xCE, 0xCB, 0xCF, 0xCC, 0xE1, 0x70, 0xDD, 0xDE, 0xDB, 0xDC, 0x8D, 0x8E, 0xDF};


int fileWrite(int fd, const char *buffer, int alet, int length, int *returnCode, int *reasonCode) {
  int zero = 0, returnValue;
  void (*bpx4wrt)(int *, const char**, int *, int *, int *, int *, int *);
  bpx4wrt = CVT_CSRT_GET(0x18, 0xDC);

  bpx4wrt(&fd, (const char **) &buffer, &zero, &length, &returnValue, returnCode, reasonCode);
  if (returnValue < 0) {
    return -1;
  } else {
    *returnCode = 0;
    *reasonCode = 0;
    return returnValue;
  }
}

static
int ext_printf(const char *format, ...) {
  char dst[1024];
  va_list arg;
  int returnCode, reasonCode;

  va_start (arg, format);
  int len = vsnprintf(dst, sizeof(dst), format, arg);
  va_end (arg);

  for (int i = 0; i < len; i ++)
    dst[i] = iso88591_to_ibm1047[(unsigned) dst[i]];

  fileWrite(1, dst, 0, len, &returnCode, &reasonCode);

  return len;
}

static char hexDigits[16] ={ '0', '1', '2', '3',
			     '4', '5', '6', '7',
			     '8', '9', 'A', 'B',
			     'C', 'D', 'E', 'F'};

static const unsigned char printableEBCDIC[256] = {
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
      int ch = printableEBCDIC[0xFF & buffer[index+pos]];
      lineBuffer[linePos++] = ch;
    }
    lineBuffer[linePos++] = '|';
    lineBuffer[linePos++] = 0;
    ext_printf("%s\n",lineBuffer);
    index += 32;
  }
}

void estaex_handler_dump(struct sdwa* sdwa_ptr, void *user_data) {
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

  void *first_ptr = (void *) (uintptr_t) sdwarc4->gprs[13];
  void *ptr = (void *) (uintptr_t) sdwarc4->gprs[13];
  void *last_valid_ptr = (void *) (uintptr_t) sdwarc4->gprs[13];

  ext_printf("\n");
  ext_printf("Stack frames:\n");
  int frames = 0;
  while (ptr && frames < 50) {
    ext_printf("Frame at %p:\n", ptr);
    char *eyecat = ((char *)ptr) + 4;
    char eyecat_true[] = { 0xc6, 0xf4, 0xe2, 0xc1 };
    if (memcmp(eyecat, eyecat_true, 4)) {
      ext_printf("  NO EYECATCHER\n");
      break;
    }
    last_valid_ptr = ptr;
    for (int i = 0; i < 16; i += 2)
      ext_printf("  %2d: 0x%016x    %2d: 0x%016x\n", (i + 14) % 16, *(((unsigned long *) ptr) + i + 1), (i + 15) % 16, *(((unsigned long *) ptr) + i + 2));
    ptr = (void*) *(((unsigned long *) ptr) + 16);
    frames ++;
  }

  ext_printf("\n");
  ext_printf("Stack dump (at 0x%016x):\n", last_valid_ptr);
  dump_buffer(last_valid_ptr, ((char*)first_ptr - (char*)last_valid_ptr) + 256);

  ext_printf("\n");
  ext_printf("*** END ESTAX DUMP ***");
  ext_printf("\n");
}
