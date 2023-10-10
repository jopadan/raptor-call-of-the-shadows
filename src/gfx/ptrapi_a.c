#include "gfxapi.h"
#include "task_man.h"
#include <stdio.h>
#include <string.h>

extern   BYTE *   displaybuffer;
extern   BYTE *   displayscreen;
extern   DWORD    cursorloopx;
extern   DWORD    cursorloopy;
extern   DWORD    joy_x;
extern   DWORD    joy_y;
extern   DWORD    joy_buttons;
extern   BYTE *   cursorstart;
extern   BYTE *   displaypic;
extern   BYTE *   cursorsave;

#define SCREENWIDTH 320
#define SCREENHEIGHT 200

#define CURSORWIDTH 16
#define CURSORHEIGHT 16

/*==========================================================================
; PTR_Save() - Saves screen before a cursor draw
 ==========================================================================*/
VOID
PTR_Save (
VOID
) {
    BYTE *src = cursorstart;
    BYTE *dst = cursorsave;
    if (!src || !dst)
        return;
    BYTE h = CURSORHEIGHT;
    while(h--) {
        memcpy(dst, src, CURSORWIDTH);
        dst += CURSORWIDTH;
        src += SCREENWIDTH;
    }
}

/*==========================================================================
; PTR_ClipSave() - Saves screen before a cursor draw
 ==========================================================================*/
VOID
PTR_ClipSave (
VOID
) {
    BYTE *src = cursorstart;
    BYTE *dst = cursorsave;
    if (!src || !dst)
        return;
    DWORD h = cursorloopy;
    while(h--) {
        memcpy(dst, src, cursorloopx);
        dst += CURSORWIDTH;
        src += SCREENWIDTH;
    }
}

/*==========================================================================
; PTR_Erase() - Erases cursor and clips edges of screen
 ==========================================================================*/
VOID
PTR_Erase (
VOID
) {
    BYTE *src = cursorsave;
    BYTE *dst = cursorstart;
    if (!src || !dst)
        return;
    BYTE h = CURSORHEIGHT;
    while(h--) {
        memcpy(dst, src, CURSORWIDTH);
        dst += SCREENWIDTH;
        src += CURSORWIDTH;
    }
}

/*==========================================================================
; PTR_ClipErase() - Erases cursor and clips edges of screen
 ==========================================================================*/
VOID
PTR_ClipErase (
VOID
) {
    BYTE *src = cursorsave;
    BYTE *dst = cursorstart;
    if (!src || !dst)
        return;
    DWORD h = cursorloopy;
    while(h--) {
        memcpy(dst, src, cursorloopx);
        dst += SCREENWIDTH;
        src += CURSORWIDTH;
    }
}
/*==========================================================================
; PTR_Draw() - Draws Cursor
 ==========================================================================*/
VOID
PTR_Draw (
VOID
) {
    BYTE *src = displaypic;
    BYTE *dst = cursorstart;
    if (!src || !dst)
        return;
    BYTE h = CURSORHEIGHT;
    while(h--) {
        BYTE w = CURSORWIDTH;
        while(w--) {
            BYTE v = *src++;
            if (v)
                *dst++ = v;
            else
                ++dst;
        }
        dst += SCREENWIDTH - CURSORWIDTH;
    }
}

VOID PTR_ReadJoyStick (VOID) {
    printf("PTR_ReadJoyStick\n");
}

