cmake_minimum_required(VERSION 3.5)

project(SDLRaptor)

find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)

add_definitions( -Wall )
add_subdirectory(src)
