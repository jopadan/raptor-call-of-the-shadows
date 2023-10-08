#include "gfxapi.h"
#include "task_man.h"
#include <stdio.h>
#include <string.h>

#define SCREENWIDTH 320
#define SCREENHEIGHT 200

extern   BYTE *   displaybuffer;
extern   BYTE *   displayscreen;

extern   BYTE *   gfx_inmem;

extern   INT      gfx_xp;
extern   INT      gfx_yp;
extern   INT      gfx_lx;
extern   INT      gfx_ly;
extern   INT      gfx_imga;

extern   INT      ud_x;
extern   INT      ud_y;
extern   INT      ud_lx;
extern   INT      ud_ly;

extern   BOOL     update_start;

extern void _dos_update_screen(void);


VOID GFX_Shade (
BYTE * outmem,             // INPUT : EAX pointer to destination
INT    maxlen,             // INPUT : EDX length of buffer to shade
BYTE * dtable              // INPUT : EBX pointer to shade table
) {
    printf("GFX_Shade\n");
}

VOID  
GFX_PutPic (
VOID
) {
    printf("GFX_PutPic %d,%d %d,%d\n", gfx_xp, gfx_yp, gfx_lx, gfx_ly);
    BYTE * src = gfx_inmem;
    BYTE * dst = displaybuffer + gfx_yp * SCREENWIDTH + gfx_xp;
    INT h = gfx_ly;
    while(h-- > 0) {
        memcpy(dst, src, gfx_lx);
        src += gfx_lx + gfx_imga;
        dst += SCREENWIDTH;
    }
}

VOID 
GFX_PutMaskPic (
VOID
) {
    printf("GFX_MaskPic\n");
}

VOID 
GFX_ShadeSprite ( 
BYTE * dest, 
BYTE * inmem, 
BYTE * dtable
) {
    printf("GFX_ShadeSprite\n");
}

VOID __cdecl
GFX_CScaleLine (
BYTE * outmem,             // INPUT : pointer to destination
BYTE * inmem               // INPUT : pointer to picture data
) {
    printf("GFX_CScaleLine\n");
}

VOID __cdecl
GFX_ScaleLine (
BYTE * outmem,             // INPUT : pointer to destination
BYTE * inmem               // INPUT : pointer to picture data
) {
    printf("GFX_CScaleLine\n");
}

VOID  
GFX_DisplayScreen (
VOID
) {
    printf("GFX_DisplayScreen %d %d %d %d\n", ud_x, ud_y, ud_lx, ud_ly);
    BYTE * src = displaybuffer + ud_y * SCREENWIDTH + ud_x;
    BYTE * dst = displayscreen + ud_y * SCREENWIDTH + ud_x;
    INT h = ud_ly;
    while(h-- > 0) {
        memcpy(dst, src, ud_lx);
        src += SCREENWIDTH;
        dst += SCREENWIDTH;
    }
    _dos_update_screen();
}

VOID 
GFX_DrawSprite ( 
BYTE * dest, 
BYTE * inmem 
) {
    printf("GFX_DrawSprite\n");
}

VOID __cdecl
GFX_DrawChar (
BYTE  * dest,              // OUTPUT: destination buffer
BYTE  * inmem,             // INPUT : source buffer
INT     width,             // INPUT : width of character
INT     height,            // INPUT : height of character
INT     addx,              // INPUT : addx offset
INT     color              // INPUT : base color
) {
    printf("GFX_DrawChar %d %d %d %d\n", width, height, addx, color);
}


