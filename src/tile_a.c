#include "tile.h"
#include <stdio.h>
#include <string.h>

// TILE

#define SCREENWIDTH     320
#define SCREENHEIGHT    200

#define TILEWIDTH       32
#define TILEHEIGHT      32
#define MAP_LEFT        16;

extern INT tileloopy;
extern BYTE * displaybuffer;
extern BYTE * displayscreen;
extern INT g_mapleft;

extern BYTE * tilepic;
extern BYTE * tilestart;

extern void _dos_update_screen(void);

VOID TILE_Draw ( VOID ) {
    BYTE *src = tilepic;
    BYTE *dst = tilestart;
    BYTE h = TILEHEIGHT;
    while(h--) {
        memcpy(dst, src, TILEWIDTH);
        src += TILEWIDTH;
        dst += SCREENWIDTH;
    }
}

VOID TILE_ShakeScreen(VOID) {
    BYTE *src = displaybuffer;
    BYTE *dst = displayscreen;
    src += MAP_LEFT - 4;
    dst += g_mapleft - 4;
    int h = 200;
    while(h--) {
        memcpy(dst, src, 296);
        dst += 296 + 24;
        src += 296 + 24;
    }
    _dos_update_screen();
}

VOID TILE_DisplayScreen(VOID) {
    BYTE *src = displaybuffer;
    BYTE *dst = displayscreen;
    src += g_mapleft;
    dst += g_mapleft;
    int h = 200;
    while(h--) {
        memcpy(dst, src, 288);
        dst += 288 + 32;
        src += 288 + 32;
    }
    _dos_update_screen();
}

VOID TILE_ClipDraw ( VOID ) {
    BYTE *src = tilepic;
    BYTE *dst = tilestart;
    INT h = tileloopy;
    while(h--) {
        memcpy(dst, src, TILEWIDTH);
        src += TILEWIDTH;
        dst += SCREENWIDTH;
    }
}

