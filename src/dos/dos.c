#include "dos.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>
#include "types.h"
#include "pcfx.h"
#include "fx_man.h"
#include "music.h"

#include <SDL2/SDL.h>
#include <assert.h>

int _dpmi_dosalloc(unsigned short size, uintptr_t *segment) {
    printf("_dpmi_dosalloc %u\n", size);
    void * ptr = calloc(1, size);
    if (!ptr)
        return -1;
    uintptr_t uptr = (uintptr_t)ptr;
    assert((uptr & 0x0f) == 0);
    *segment = uptr >> 4;
    return 0;
}
int _dpmi_getmemsize(void) {
    return 4096 * 1024;
}

int _dpmi_lockregion(void *address, unsigned length) {
    printf("_dpmi_lockregion %p %u\n", address, length);
    return 0;
}
int _dpmi_unlockregion(void *address, unsigned length) {
    printf("_dpmi_unlockregion %p %u\n", address, length);
    return 0;
}

void segread(struct SREGS *segregs) {
    memset(segregs, 0, sizeof(*segregs));
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
    printf("INT386 %02x ah=%02x\n", inter_no, in_regs->h.ah);
    *out_regs = *in_regs;
    switch(inter_no)
    {
        case 0x10:
            switch(in_regs->h.ah)
            {
                case 0x00:
                    printf("int 0x10: SetVideoMode 0x%02x\n", in_regs->h.al);
                    break;
                case 0x02:
                    printf("int 0x10: SetCursorPosition %u %u %u\n", in_regs->h.bh, in_regs->h.dh, in_regs->h.dl);
                    break;
                default:
                    abort();
            }
            break;
        case 0x33:
            switch(in_regs->h.ah)
            {
                case 0x00: {
                    printf("int 0x33: GetMouseInfo\n");
                    out_regs->w.ax = 0;
                    out_regs->w.bx = 3;
                    break;
                }
                default:
                    abort();
            }
            break;
        default:
            abort();
    }
    return out_regs->x.eax;
}

int inp(unsigned short port) {
    printf("INP %04x\n", port);
    return 0;
}

int outp(
   unsigned short port,
   int data_byte
) {
    printf("OUTP %04x %d\n", port, data_byte);
    return data_byte;
}

void _dos_setvect(unsigned intnum, void (__cdecl _interrupt _far *handler)()) {
    printf("DOSSETVECT %u\n", intnum);
}

void (__cdecl _interrupt _far *_dos_getvect(unsigned intnum))() {
    printf("_dos_getvect %u\n", intnum);
    return NULL;
}

int strcmpi(const char *str1, const char *str2) {
    if (str1) {
        if (str2)
            return strcasecmp(str1, str2);
        else
            return 1;
    } else {
        return -1;
    }
}

char * strupr(char * str) {
    char *p = str;
    while(*p) {
        *p = toupper(*p);
        ++p;
    }
    return str;
}

long filelength(int fd) {
    struct stat stat;
    if (fstat(fd, &stat) == 0) {
        return stat.st_size;
    }
    return -1;
}

int chsize( int handle, long size ) {
    return ftruncate(handle, size);
}

char * ltoa(long l, char * buffer, int radix) {
    return SDL_itoa(l, buffer, radix);
}

//GFX

/*==========================================================================
; PTR_Save() - Saves screen before a cursor draw
 ==========================================================================*/
VOID
PTR_Save (
VOID
) {
    printf("PTR_Save\n");
}

/*==========================================================================
; PTR_ClipSave() - Saves screen before a cursor draw
 ==========================================================================*/
VOID
PTR_ClipSave (
VOID
) {
    printf("PTR_ClipSave\n");
}

/*==========================================================================
; PTR_Erase() - Erases cursor and clips edges of screen
 ==========================================================================*/
VOID
PTR_Erase (
VOID
) {
    printf("PTR_Erase\n");
}

/*==========================================================================
; PTR_ClipErase() - Erases cursor and clips edges of screen
 ==========================================================================*/
VOID
PTR_ClipErase (
VOID
) {
    printf("PTR_ClipErase\n");
}
/*==========================================================================
; PTR_Draw() - Draws Cursor
 ==========================================================================*/
VOID
PTR_Draw (
VOID
) {
    printf("PTF_Draw\n");
}

VOID PTR_ReadJoyStick (VOID) {
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

VOID TILE_Draw ( VOID ) {
    printf("TILE_Draw\n");
}

VOID TILE_ShakeScreen(VOID) {
    printf("TILE_ShakeScreen\n");
}
VOID TILE_DisplayScreen(VOID) {
    printf("TILE_ShakeScreen\n");
}
VOID TILE_ClipDraw ( VOID ) {
    printf("TILE_ClipDraw\n");
}

VOID
ANIM_Render (
BYTE * inmem
) {
    printf("ANIM_Render\n");
}

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
    printf("GFX_PutPic\n");
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
    printf("GFX_DisplayScreen\n");
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


//MUSIC

int   MUSIC_Init( int SoundCard, int Address ) {
    return MUSIC_Ok;
}

void  MUSIC_Continue( void ) {
    printf("MUSIC_Continue\n");
}

void  MUSIC_Pause( void ) {
    printf("MUSIC_Pause\n");
}

void  MUSIC_SetVolume( int volume ) {
    printf("MUSIC_SetVolume %d\n", volume);
}

int   MUSIC_SongPlaying( void ) {
    printf("MUSIC_SongPlaying\n");
    return 0;
}

int   MUSIC_StopSong( void ) {
    printf("MUSIC_StopSong\n");
    return 0;
}

int   MUSIC_PlaySong( unsigned char *song, int loopflag ) {
    printf("MUSIC_PlaySong %s, loop: %d\n", song, loopflag);
    return 0;
}

int   MUSIC_FadeVolume( int tovolume, int milliseconds ) {
    printf("MUSIC_FadeVolume %d %d\n", tovolume, milliseconds);
    return 0;
}

int   MUSIC_FadeActive( void ) {
    printf("MUSIC_FadeActive\n");
    return 0;
}

int   MUSIC_Shutdown( void ) {
    printf("MUSIC_Shutdown\n");
    return 0;
}

int   PCFX_Init( void ) {
    printf("PCFX_Init\n");
    return 0;
}

int   PCFX_Shutdown( void ) {
    printf("PCFX_Shutdown\n");
    return 0;
}

int   PCFX_SetTotalVolume( int volume ) {
    printf("PCFX_SetTotalVolume %d\n", volume);
    return 0;
}

void  PCFX_UseLookup( int use, unsigned value ) {
    printf("PCFX_UseLookup %d, %d", use, value);
}

int   PCFX_Play( PCSound *sound, int priority, unsigned long callbackval ) {
    printf("PCFX_Play\n");
    return 0;
}

int   PCFX_Stop( int handle ) {
    printf("PCFX_Stop\n");
    return 0;
}

int   PCFX_SoundPlaying( int handle ) {
    printf("PCFX_SoundPlaying %d\n", handle);
    return 0;
}

int FX_SoundActive( int handle ) {
    printf("PCFX_SoundActive %d\n", handle);
    return 0;
}

int FX_SetPan( int handle, int vol, int left, int right ) {
    printf("FX_SetPan %d %d %d %d\n", handle, vol, left, right);
    return 0;
}

int FX_SetPitch( int handle, int pitchoffset ) {
    printf("FX_SetPitch %d %d\n", handle, pitchoffset);
}


int FX_PlayRaw( char *ptr, unsigned long length, unsigned rate,
       int pitchoffset, int vol, int left, int right, int priority,
       unsigned long callbackval ) {
    printf("FX_PlayRaw\n");
    return 0;
}

int FX_StopSound( int handle ) {
    printf("FX_StopSound %d\n", handle);
    return 0;
}

int   FX_GetBlasterSettings( fx_blaster_config *blaster ) {
    return 1;
}

int AL_DetectFM(void) {
    return 0;
}

int MPU_Init(int addr) {
    return 0;
}

int   FX_SetupSoundBlaster( fx_blaster_config blaster, int *MaxVoices, int *MaxSampleBits, int *MaxChannels ) {
    printf("FX_SetupSoundBlaster\n");
    return 0;
}

int   FX_Init( int SoundCard, int numvoices, int numchannels, int samplebits, unsigned mixrate ) {
    printf("FX_Init %d %d %d %d %d\n", SoundCard, numchannels, numchannels, samplebits, mixrate);
    return 0;
}

int   FX_Shutdown( void ) {
    printf("FX_Shutdown\n");
    return 0;
}

void  FX_SetVolume( int volume ) {
    printf("FX_SetVolume %d\n", volume);
}