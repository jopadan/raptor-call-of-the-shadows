cmake_minimum_required(VERSION 3.5)

project(raptor)

set(CMAKE_OSX_ARCHITECTURES arm64;x86_64)

add_subdirectory(src)

install(FILES setup.ini DESTINATION bin)
