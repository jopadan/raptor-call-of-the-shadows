add_library(rapcore STATIC
    rap.c
    tile.c
    tile_a.c
    shots.c
    eshot.c
    shadows.c
    enemy.c
    flame.c
    objects.c
    bonus.c
    fx.c
    input.c
    anims.c
    movie.c
    store.c
    intro.c
    loadsave.c
    help.c
    windows.c
    shadows.c

    gfx/exitapi.c
    gfx/imsapi.c
    gfx/glbapi.c
    gfx/gfxapi.c
    gfx/gfxapi_a.c
    gfx/kbdapi.c
    gfx/prefapi.c
    gfx/ptrapi.c
    gfx/ptrapi_a.c
    gfx/swdapi.c
    gfx/tsmapi.c
    gfx/vmemapi.c

    apodmx/dmx.c
    apodmx/mus2mid.c
)
target_include_directories(rapcore PUBLIC dos gfx apodmx)
target_compile_definitions(rapcore PUBLIC -D_loadds= -Dfar= -D_far= -D_interrupt= -D__cdecl= -Dcdecl= )
target_compile_definitions(rapcore PRIVATE -Dstricmp=strcmpi -Dmain=_dos_main -Dopen=_dos_open -Daccess=_dos_access -Dfopen=_dos_fopen)

add_executable(rap
    dos/dos.c
)

target_link_libraries(rap PRIVATE rapcore SDL2_mixer SDL2)

install(TARGETS rap)
