#ifndef DOS_DOS_H__
#define DOS_DOS_H__

#include <string.h>
#include <errno.h>
#include <stdint.h>
#include "task_man.h"

#include <limits.h>
#define _MAX_PATH PATH_MAX

struct DWORDREGS {
  uint32_t edi;
  uint32_t esi;
  uint32_t ebp;
  uint32_t cflag;
  uint32_t ebx;
  uint32_t edx;
  uint32_t ecx;
  uint32_t eax;
  unsigned short eflags;
} __attribute__((packed));

struct WORDREGS {
  uint16_t di, _upper_di;
  uint16_t si, _upper_si;
  uint16_t bp, _upper_bp;
  uint16_t cflag, _upper_cflag;
  uint16_t bx, _upper_bx;
  uint16_t dx, _upper_dx;
  uint16_t cx, _upper_cx;
  uint16_t ax, _upper_ax;
  uint16_t flags;
} __attribute__((packed));

struct BYTEREGS {
  uint16_t di, _upper_di;
  uint16_t si, _upper_si;
  uint16_t bp, _upper_bp;
  uint32_t cflag;
  uint8_t bl;
  uint8_t bh;
  uint16_t _upper_bx;
  uint8_t dl;
  uint8_t dh;
  uint16_t _upper_dx;
  uint8_t cl;
  uint8_t ch;
  uint16_t _upper_cx;
  uint8_t al;
  uint8_t ah;
  uint16_t _upper_ax;
  uint16_t flags;
} __attribute__((packed));

union REGS {
  struct DWORDREGS x;
  struct WORDREGS w;
  struct BYTEREGS h;
} __attribute__((packed));

struct SREGS {
  unsigned short es;
  unsigned short ds;
  unsigned short fs;
  unsigned short gs;
  unsigned short cs;
  unsigned short ss;
};

void segread(struct SREGS *segregs);

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
int int386x( int inter_no,
            const union REGS *in_regs,
            union REGS *out_regs, struct SREGS * sregs);

uint32_t FP_OFF(uintptr_t ptr);
uint16_t FP_SEG(uintptr_t ptr);
uintptr_t FP_PTR(uint32_t ptr, uint16_t seg);

int inp(
   unsigned short port
);

int outp(
   unsigned short port,
   int data_byte
);

int chsize( int handle, long size );
char * ltoa(long l, char * buffer, int radix);

void _dos_setvect(unsigned intnum, void (*handler)());
void (*_dos_getvect(unsigned intnum))();

extern uint8_t _dos_video_ram[];

int strcmpi(const char *, const char *);

void _dos_update_screen();

#endif
