add_executable(rap
    rap.c
    tile.c
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
    gfx/kbdapi.c
    gfx/prefapi.c
    gfx/ptrapi.c
    gfx/swdapi.c
    gfx/tsmapi.c
    gfx/vmemapi.c

    apodmx/dmx.c
    apodmx/mus2mid.c

    dos/dos.c
)

target_link_libraries(rap PRIVATE SDL2)
target_compile_definitions(rap PRIVATE -D_loadds= -Dfar= -D_far= -D_interrupt= -D__cdecl= -Dcdecl= -Dstricmp=strcmpi)
target_include_directories(rap PRIVATE dos gfx apodmx)
