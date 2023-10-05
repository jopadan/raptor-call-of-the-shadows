#ifndef DOS_DOS_H__
#define DOS_DOS_H__

#include <string.h>
#include <errno.h>

#include <limits.h>
#define _MAX_PATH PATH_MAX

struct DWORDREGS {
  unsigned long edi;
  unsigned long esi;
  unsigned long ebp;
  unsigned long cflag;
  unsigned long ebx;
  unsigned long edx;
  unsigned long ecx;
  unsigned long eax;
  unsigned short eflags;
};

struct WORDREGS {
  unsigned short di, _upper_di;
  unsigned short si, _upper_si;
  unsigned short bp, _upper_bp;
  unsigned short cflag, _upper_cflag;
  unsigned short bx, _upper_bx;
  unsigned short dx, _upper_dx;
  unsigned short cx, _upper_cx;
  unsigned short ax, _upper_ax;
  unsigned short flags;
};

struct BYTEREGS {
  unsigned short di, _upper_di;
  unsigned short si, _upper_si;
  unsigned short bp, _upper_bp;
  unsigned long cflag;
  unsigned char bl;
  unsigned char bh;
  unsigned short _upper_bx;
  unsigned char dl;
  unsigned char dh;
  unsigned short _upper_dx;
  unsigned char cl;
  unsigned char ch;
  unsigned short _upper_cx;
  unsigned char al;
  unsigned char ah;
  unsigned short _upper_ax;
  unsigned short flags;
};

union REGS {
  struct DWORDREGS d;
  struct WORDREGS w;
  struct BYTEREGS h;
};

struct dosdate_t
      {
      unsigned char day;        // - [1-31]
      unsigned char month;      // - [1-12]
      unsigned int  year;       // - [1980-2099]
      unsigned char dayofweek;  // - [0-6] = Sun..Sat
      };

void _dos_getdate( struct dosdate_t *date );
void _disable(void);
void _enable(void);
int int386( int inter_no,
            const union REGS *in_regs,
            union REGS *out_regs );

int inp(
   unsigned short port
);

int outp(
   unsigned short port,
   int data_byte
);

#endif
