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
)

target_link_libraries(rap PRIVATE SDL2)
target_compile_definitions(rap PRIVATE -D__cdecl= )
target_include_directories(rap PRIVATE dos gfx apodmx)
