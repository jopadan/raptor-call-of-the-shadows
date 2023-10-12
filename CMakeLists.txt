cmake_minimum_required(VERSION 3.5)

project(raptor)

add_subdirectory(src)

install(FILES setup.ini DESTINATION bin)
