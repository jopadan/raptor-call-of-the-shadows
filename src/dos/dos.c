#include "dos.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "types.h"

int _dpmi_dosalloc(unsigned short size, unsigned int *segment) {
    return 0;
}
int _dpmi_getmemsize(void) {
    return 4096 * 1024;
}

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

//GFX

VOID  
PTR_ReadJoyStick (
VOID
) {
    printf("PTR_ReadJoyStick\n");
}

//TSM

void TSM_Install(int rate) {
    printf("TSM_Install %d\n", rate);
}
int TSM_NewService(void(*function)(void), int rate, int priority, int pause) {
    printf("TSM_NewService %d %d %d\n", rate, priority, rate);
}
void TSM_DelService(int id) {
    printf("TSM_DelService %d\n", id);
}
void TSM_Remove(void) {
    printf("TSM_Remove\n");
}
void TSM_PauseService(int id) {
    printf("TSM_PauseService %d\n", id);
}
void TSM_ResumeService(int id) {
    printf("TSM_ResumeService %d\n", id);
}

// TILE
void TILE_ShakeScreen() {
    printf("TILE_ShakeScreen\n");
}
void TILE_DisplayScreen() {
    printf("TILE_ShakeScreen\n");
}