#include "dos.h"
#include <fcntl.h>
#include <time.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include "types.h"
#include "pcfx.h"
#include "fx_man.h"
#include "music.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <assert.h>


#define GAME_MIX_FREQ (11025)
#define GAME_MIX_FORMAT (AUDIO_U8)

static SDL_Window * sdl_window = NULL;
static SDL_Renderer * sdl_renderer = NULL;
static SDL_Surface * sdl_surface = NULL;
static SDL_Palette * sdl_palette = NULL;
static Mix_Music * sdl_music = NULL;
static int zoom = 3;
static uint8_t palette[0x300];
static bool palette_updated = FALSE;
static unsigned palette_index_r = 0;
static unsigned palette_index_w = 0;
static unsigned char retrace = 0;
typedef void (*_dos_int_handler)();
static _dos_int_handler int_handlers[256];
static uint8_t kbd_code[2];
static uint8_t kbd_code_r = 0;
VOID (*mouse_handler)(INT, INT, INT) = NULL;

uint8_t _dos_video_ram[0x12c00];

uint32_t FP_OFF(uintptr_t ptr) {
    assert((ptr >> 48) == 0);
    return ptr;
}

uint16_t FP_SEG(uintptr_t ptr) {
    assert((ptr >> 48) == 0);
    return ptr >> 32;
}

uintptr_t FP_PTR(uint32_t ptr, uint16_t seg) {
    return ptr | ((uintptr_t)seg << 32);
}


int _dpmi_dosalloc(unsigned short size, uintptr_t *segment) {
    size *= 16;
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

int int386x( int inter_no,
            const union REGS *in_regs,
            union REGS *out_regs, struct SREGS *sregs ) {
    *out_regs = *in_regs;
    switch(inter_no)
    {
        case 0x10:
            switch(in_regs->h.ah)
            {
                case 0x00:
                    printf("int 0x10: SetVideoMode 0x%02x\n", in_regs->h.al);
                    if (in_regs->h.al == 0x13) {
                        assert(sdl_window == NULL);
                        sdl_window = SDL_CreateWindow("RAPTOR: Call Of The Shadows V1.2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 320 * zoom, 200 * zoom, 0);
                        if (!sdl_window) {
                            printf("SDL_CreateWindow: %s", SDL_GetError());
                            abort();
                        }
                        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
                        sdl_renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
                        if (!sdl_renderer) {
                            printf("SDL_CreateRenderer: %s", SDL_GetError());
                            abort();
                        }
                        sdl_palette = SDL_AllocPalette(256);
                        if (!sdl_palette) {
                            printf("SDL_CreateRenderer: %s", SDL_GetError());
                            abort();
                        }
                        sdl_surface = SDL_CreateRGBSurface(0, 320, 200, 8, 0, 0, 0, 0);
                        if (!sdl_surface) {
                            printf("SDL_CreateRGBSurfaceWithFormat: %s\n", SDL_GetError());
                            abort();
                        }
                        SDL_SetSurfacePalette(sdl_surface, sdl_palette);
                    }
                    break;
                case 0x02:
                    printf("int 0x10: SetCursorPosition %u %u %u\n", in_regs->h.bh, in_regs->h.dh, in_regs->h.dl);
                    break;
                default:
                    abort();
            }
            break;
        case 0x33:
            switch(in_regs->w.ax)
            {
                case 0x00:
                    printf("int 0x33: GetMouseInfo\n");
                    out_regs->w.ax = 0xffffu;
                    out_regs->w.bx = 3;
                    break;
                case 0x01:
                    SDL_ShowCursor(SDL_ENABLE);
                    break;
                case 0x02:
                    SDL_ShowCursor(SDL_DISABLE);
                    break;
                case 0x04:
                    if (sdl_window)
                        SDL_WarpMouseInWindow(sdl_window, in_regs->w.cx * zoom, in_regs->w.dx * zoom);
                    break;
                case 0x0c: {
                    assert(sregs != NULL);
                    mouse_handler = (VOID (*)(INT, INT, INT))FP_PTR(in_regs->x.edx, sregs->es);
                    break;
                }
                default:
                    printf("int 0x33: ax = %04x\n", in_regs->w.ax);
                    abort();
            }
            break;
        default:
            printf("int 0x33: ah=%02x\n", in_regs->h.ah);
            abort();
    }
    return out_regs->x.eax;
}

int int386( int inter_no,
            const union REGS *in_regs,
            union REGS *out_regs) {
    return int386x(inter_no, in_regs, out_regs, NULL);
}

uint8_t _dos_keycode(uint8_t code, bool release) {
    return code | (release? 0x80: 0x00);
}

void _dos_translate_key(SDL_Scancode code, bool release) {
    kbd_code_r = 0;
    kbd_code[0] = kbd_code[1] = 0;
    switch(code) {
    case SDL_SCANCODE_ESCAPE:
        kbd_code[0] = _dos_keycode(1, release);
        break;
    case SDL_SCANCODE_1:
    case SDL_SCANCODE_2:
    case SDL_SCANCODE_3:
    case SDL_SCANCODE_4:
    case SDL_SCANCODE_5:
    case SDL_SCANCODE_6:
    case SDL_SCANCODE_7:
    case SDL_SCANCODE_8:
    case SDL_SCANCODE_9:
    case SDL_SCANCODE_0:
        kbd_code[0] = _dos_keycode(2 + code - SDL_SCANCODE_1, release);
        break;
    default:
        printf("KBD: missing scancode for %u\n", code);
        return;
    }
    if (int_handlers[9] != NULL)
        int_handlers[9]();
}


void _dos_update_screen() {
    printf("_dos_update_screen\n");
    if (!sdl_window)
        return;

    SDL_Event event;
    bool callMouseHandler = true;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            _dos_translate_key(event.key.keysym.scancode, event.type == SDL_KEYUP);
            break;
        case SDL_MOUSEMOTION:
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEBUTTONDOWN:
            callMouseHandler = true;
            break;
        case SDL_QUIT:
            printf("Quit requested\n");
            exit(0);
            break;
        }
    }
    if (callMouseHandler && mouse_handler) {
        int x, y;
        Uint32 buttons = SDL_GetMouseState(&x, &y);
        INT dos_buttons =
            ((buttons & SDL_BUTTON_LMASK)? 1: 0) |
            ((buttons & SDL_BUTTON_RMASK)? 2: 0) |
            ((buttons & SDL_BUTTON_MMASK)? 4: 0);
        mouse_handler(dos_buttons, x * 2 / zoom, y / zoom);
    }
    if (palette_updated) {
        SDL_Color colors[256];
        const BYTE *src = palette;
        for(unsigned i = 0; i != 256; ++i) {
            SDL_Color *color = colors + i;
            color->r = *src++;
            color->g = *src++;
            color->b = *src++;
        }
        if (SDL_SetPaletteColors(sdl_palette, colors, 0, 256)) {
            printf("SDL_SetPaletteColors: %s\n", SDL_GetError());
            abort();
        }
        palette_updated = FALSE;
    }
    bool must_lock = SDL_MUSTLOCK(sdl_surface);
    if (must_lock) {
        if (SDL_LockSurface(sdl_surface) != 0) {
            printf("SDL_LockSurface: %s\n", SDL_GetError());
            abort();
        }
    }
    BYTE *dst = (BYTE *)sdl_surface->pixels;
    assert(dst);
    const BYTE * src = _dos_video_ram;
    int h = 200;
    while(h--) {
        memcpy(dst, src, 320);
        dst += sdl_surface->pitch;
        src += 320;
    }
    if (must_lock)
        SDL_UnlockSurface(sdl_surface);

    SDL_Surface * window_surface = SDL_GetWindowSurface(sdl_window);
    if (!window_surface) {
        printf("SDL_GetWindowSurface: %s\n", SDL_GetError());
        abort();
    }

    SDL_BlitSurface(sdl_surface, NULL, window_surface, NULL);

    SDL_RenderClear(sdl_renderer);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(sdl_renderer, sdl_surface);
    if (!tex) {
        printf("SDL_CreateTextureFromSurface: %s\n", SDL_GetError());
        abort();
    }
    SDL_RenderCopy(sdl_renderer, tex, NULL, NULL);
    SDL_DestroyTexture(tex);
    SDL_RenderPresent(sdl_renderer);
}

int inp(unsigned short port) {
    switch(port)
    {
    case 0x60:
        return kbd_code_r < sizeof(kbd_code)? kbd_code[kbd_code_r++]: 0;
    case 0x3DA:
        retrace ^= 8;
        return retrace;
    case 0x3c9:
        return palette[palette_index_r++] >> 2;
    default:
        printf("reading from port 0x%04x\n", port);
        abort();
    }
}

int outp(
   unsigned short port,
   int data_byte
) {
    switch(port) {
    case 0x20: //PIC1 control
        break;
    case 0x3c7:
        palette_index_r = data_byte * 3;
        break;
    case 0x3c8:
        palette_index_w = data_byte * 3;
        break;
    case 0x3c9:
        palette[palette_index_w++] = data_byte << 2;
        palette_updated = true;
        if (palette_index_w == sizeof(palette)) {
            _dos_update_screen();
        }
        break;
    default:
        printf("writing 0x%02x to port 0x%04x\n", data_byte, port);
        abort();
    }
    return data_byte;
}

void _dos_setvect(unsigned intnum, void (*handler)()) {
    printf("_dos_setvect %u\n", intnum);
    assert(intnum < 256);
    int_handlers[intnum] = handler;
}

void (*_dos_getvect(unsigned intnum))() {
    assert(intnum < 256);
    return int_handlers[intnum];
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

//TSAPI
#define MAX_TASKS (8)

struct ts_task {
    struct task task;
    SDL_TimerID timer;
};

static Uint32 SDLCALL TS_Callback (Uint32 interval, void *param) {
    struct ts_task * ts_task = (struct ts_task *)param;
    ts_task->task.TaskService(&ts_task->task);
    return interval;
}

struct task *TS_ScheduleTask( void ( *Function )( task * ), int rate,
                          int priority, void *data ) {
  int interval = (1000 + rate - 1) / rate;
  printf("TS_ScheduleTask %d(%dms) %d\n", rate, interval, priority);
  struct ts_task * ts_task = (struct ts_task *)calloc(sizeof(struct ts_task), 1);
  struct task *task = &ts_task->task;
  task->TaskService = Function;
  task->rate = rate;
  task->priority = priority;
  task->active = TRUE;
  task->data = data;
  ts_task->timer = SDL_AddTimer(interval, &TS_Callback, ts_task);
  if (!ts_task->timer) {
    printf("SDL_AddTimer failed: %s\n", SDL_GetError());
    abort();
  }
  return task;
}

int TS_Terminate( struct task *task ) {
    struct ts_task * ts_task = (struct ts_task *)task;
    printf("TS_Terminate\n");
    SDL_RemoveTimer(ts_task->timer);
    free(ts_task);
    return 0;
}

void TS_Shutdown( void ) {
}

void TS_Dispatch() {
}


// TILE

VOID TILE_Draw ( VOID ) {
    printf("TILE_Draw\n");
}

VOID TILE_ShakeScreen(VOID) {
    printf("TILE_ShakeScreen\n");
}
VOID TILE_DisplayScreen(VOID) {
    printf("TILE_DisplayScreen\n");
}
VOID TILE_ClipDraw ( VOID ) {
    printf("TILE_ClipDraw\n");
}

//MUSIC

int   MUSIC_Init( int SoundCard, int Address ) {
    printf("MUSIC_Init %d %d\n", SoundCard, Address);
    int r = Mix_Init(MIX_INIT_MID);
    printf("MIX_Init: %08x\n", r);
    if (Mix_OpenAudio(GAME_MIX_FREQ, GAME_MIX_FORMAT, MIX_DEFAULT_CHANNELS, 16)) {
        printf("Mix_OpenAudio: %s\n", SDL_GetError());
        abort();
    }
    return MUSIC_Ok;
}

void  MUSIC_Continue( void ) {
    Mix_ResumeMusic();
}

void  MUSIC_Pause( void ) {
    Mix_PauseMusic();
}

void  MUSIC_SetVolume( int volume ) {
    printf("MUSIC_SetVolume %d\n", volume);
    Mix_VolumeMusic(volume * MIX_MAX_VOLUME / 255);
}

int   MUSIC_SongPlaying( void ) {
    printf("MUSIC_SongPlaying\n");
    // FIXME: does not work for midi:
    // return Mix_PlayingMusic()? 1: 0;
    return 0;
}

int   MUSIC_StopSong( void ) {
    Mix_HaltMusic();
    return 0;
}

int   MUSIC_PlaySong( unsigned char *song, int song_len, int loopflag ) {
    SDL_RWops *rw = SDL_RWFromMem(song, song_len);
    if (!rw) {
        printf("SDL_RWFromMem: %s\n", SDL_GetError());
        abort();
    }
    sdl_music = Mix_LoadMUS_RW(rw, 1);
    if (!sdl_music) {
        printf("Mix_LoadMUS_RW: %s\n", SDL_GetError());
        abort();
    }
    if (Mix_PlayMusic(sdl_music, loopflag) != 0) {
        printf("Mix_PlayMusic: %s\n", SDL_GetError());
        abort();
    }
    return MUSIC_Ok;
}

int   MUSIC_FadeVolume( int tovolume, int milliseconds ) {
    printf("MUSIC_FadeVolume STUB %d %d\n", tovolume, milliseconds);
    MUSIC_SetVolume(tovolume);
    return 0;
}

int   MUSIC_FadeActive( void ) {
    printf("MUSIC_FadeActive\n");
    return 0;
}

int   MUSIC_Shutdown( void ) {
    printf("MUSIC_Shutdown\n");
    Mix_CloseAudio();
    Mix_Quit();
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
    return Mix_Playing(handle) > 0;
}

int FX_SoundActive( int handle ) {
    int r = Mix_Playing(handle);
    return r > 0;
}

int FX_SetPan( int handle, int vol, int left, int right ) {
    Mix_SetPanning(handle, left, right);
    Mix_Volume(handle, vol * MIX_MAX_VOLUME / 255);
    return 0;
}

int FX_SetPitch( int handle, int pitchoffset ) {
    printf("FX_SetPitch %d %d\n", handle, pitchoffset);
    return 0;
}

#define MAX_FX_HANDLES (8)

struct fx_handle {
    Mix_Chunk *chunk;
    Uint8 *buf;
    int priority;
};

static struct fx_handle fx_handles[MAX_FX_HANDLES];

static int FX_FindFreeHandle() {
    int candidates[MAX_FX_HANDLES];
    int candidates_n = 0;
    int candidate_prio = -1;
    for(int i = 0; i < MAX_FX_HANDLES; ++i) {
        struct fx_handle *fx = fx_handles + i;
        if (fx->chunk) {
            if (candidate_prio == -1 || fx->priority < candidate_prio) {
                candidates_n = 0;
                candidates[candidates_n++] = i;
                candidate_prio = fx->priority;
            } else if (fx->priority == candidate_prio) {
                candidates[candidates_n++] = i;
            }
        } else {
            printf("Allocated channel handle %d\n", i);
            return i;
        }
    }
    int idx = candidates[rand() % candidates_n];
    printf("no free channels, evicting one of %u candidate -> %d\n", candidates_n, idx);
    FX_StopSound(idx);
    return idx;
}

int FX_PlayRaw( char *ptr, unsigned long length, unsigned rate,
       int pitchoffset, int vol, int left, int right, int priority,
       unsigned long callbackval ) {
    printf("FX_PlayRaw len: %lu rate: %u pitch: %d vol: %d left: %d right:%d prio: %d\n", 
        length, rate, pitchoffset, vol, left, right, priority);
    int idx = FX_FindFreeHandle();
    if (idx < 0)
        return idx;

    SDL_AudioCVT cvt;
    int r = SDL_BuildAudioCVT(&cvt, AUDIO_U8, 1, rate, GAME_MIX_FORMAT, 1, GAME_MIX_FREQ);
    if (r < 0) {
        printf("SDL_BuildAudioCVT: %s\n", SDL_GetError());
        abort();
    }
    cvt.buf = (Uint8*)malloc(length * cvt.len_mult);
    memcpy(cvt.buf, ptr, length);
    if (r > 0) {
        if (!cvt.buf) {
            perror("malloc");
            return -1;
        }
        cvt.len = length;
        if (SDL_ConvertAudio(&cvt) < 0) {
            printf("SDL_ConvertAudio: %s", SDL_GetError());
            free(cvt.buf);
            return -1;
        }
    } else {
        cvt.len_cvt = length;
    }
    Mix_Chunk *chunk = Mix_QuickLoad_RAW(cvt.buf, cvt.len_cvt);
    if (!chunk) {
        printf("Mix_QuickLoad_RAW: %s\n", SDL_GetError());
        free(cvt.buf);
        return -1;
    }

    fx_handles[idx].buf = cvt.buf;
    fx_handles[idx].chunk = chunk;
    fx_handles[idx].priority = priority;
    Mix_SetPanning(idx, left, right);
    Mix_PlayChannel(idx, chunk, 0);
    return idx;
}

int FX_StopSound( int handle ) {
    printf("FX_StopSound %d\n", handle);
    assert(handle < MAX_FX_HANDLES);
    struct fx_handle * fx = fx_handles + handle;
    if (!fx->chunk) {
        printf("No FX chunk\n");
        return -1;
    }
    Mix_FreeChunk(fx->chunk);
    free(fx->buf);
    fx->chunk = 0;
    fx->buf = 0;
    fx->priority = 0;
    return 0;
}

int   FX_GetBlasterSettings( fx_blaster_config *blaster ) {
    return 1;
}

int AL_DetectFM(void) {
    return 1;
}

int MPU_Init(int addr) {
    return 1;
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

extern int _dos_main(int argv, char **argc);

static char * convert_path(const char *src) {
    char *dst = strdup(src);
    for(char *p = dst; *p; ++p) {
        if (*p == '/')
            *p = '\\';
    }
    return dst;
}

static char *resolve_path(const char *path) {
    size_t p = 0;
    size_t n = strlen(path);
    char *dst = strdup(path);
    while(p < n)
    {
        const char *next_ptr = strchr(path + p, '\\');
        size_t next = next_ptr? next_ptr - path: n;
        if (next < n)
            dst[next] = 0;
        if (next > p) {
            if (access(dst, F_OK) != 0) {
                char backup = dst[p];
                dst[p] = 0;
                DIR *dir = opendir(p != 0? dst: ".");
                dst[p] = backup;
                if (dir) {
                    struct dirent *de;
                    bool found = false;
                    while((de = readdir(dir)) != NULL) {
                        if (strcasecmp(dst + p, de->d_name) == 0) {
                            strncpy(dst + p, de->d_name, strlen(de->d_name));
                            found = true;
                            break;
                        }
                    }
                    closedir(dir);
                    if (!found) {
                        free(dst);
                        return NULL;
                    }
                } else {
                    free(dst);
                    return NULL;
                }
            }
        }
        if (next < n)
            dst[next] = '/';
        p = next + 1;
    }
    return dst;
}

int _dos_open(const char *path, int mode) {
    char *resolved = resolve_path(path);
    if (!resolved) {
        errno = ENOENT;
        return -1;
    }
    int r = open(resolved, mode);
    free(resolved);
    return r;
}

int _dos_access(const char *path, int mode) {
    char *resolved = resolve_path(path);
    if (!resolved) {
        errno = ENOENT;
        return -1;
    }
    int r = access(resolved, mode);
    free(resolved);
    return r;
}

int main(int argc, char **argv) {
    if (SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_NOPARACHUTE)) {
        printf("SDL_Init: %s\n", SDL_GetError());
        exit(1);
    }

    argv[0] = convert_path(argv[0]);
    int ret = _dos_main(argc, argv);
    return ret;
}
