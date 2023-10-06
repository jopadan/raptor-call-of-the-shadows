#include "dos.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "types.h"

void _dos_getdate( struct dosdate_t *date ) {
    time_t t = 0;
    struct tm tm;
    time(&t);
    localtime_r(&t, &tm);
    date->dayofweek = tm.tm_wday;
    date->day = tm.tm_mday;
    date->month = tm.tm_mon + 1;
    date->year = tm.tm_year + 1900;
}

int rap_random(int x) {
    return rand() % x;
}

void _disable(void) {}
void _enable(void) {}
int int386( int inter_no,
            const union REGS *in_regs,
            union REGS *out_regs ) {
    printf("INT386 %u\n", inter_no);
    abort();
}

int inp(unsigned short port) {
    printf("INP %u\n", port);
    return 0;
}

int outp(
   unsigned short port,
   int data_byte
) {
    printf("OUTP %u %d\n", port, data_byte);
    return data_byte;
}

void _dos_setvect(unsigned intnum, void (__cdecl _interrupt _far *handler)()) {
    printf("DOSSETVECT %u\n", intnum);
}

//GFX PART
PUBLIC INT     cur_mx         = 0;
PUBLIC INT     cur_my         = 0;
PUBLIC INT     mouseb1        = 0;
PUBLIC INT     mouseb2        = 0;

PUBLIC BOOL    mouse_b1_ack   = FALSE;
PUBLIC BOOL    mouse_b2_ack   = FALSE;
PUBLIC BOOL    mouse_b3_ack   = FALSE;

PUBLIC  BOOL   ptr_init_flag  = FALSE;


PUBLIC  INT    joy_limit_xh   = 10;
PUBLIC  INT    joy_limit_xl   = -10;
PUBLIC  INT    joy_limit_yh   = 10;
PUBLIC  INT    joy_limit_yl   = -10;

PUBLIC  INT    joy_x          = 0;
PUBLIC  INT    joy_y          = 0;
PUBLIC  INT    joy_sx         = 0;
PUBLIC  INT    joy_sy         = 0;

PUBLIC  INT    joy_buttons    = 0;
