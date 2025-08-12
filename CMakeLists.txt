cmake_minimum_required(VERSION 3.7)

project(test C)

add_executable(ffdefault WIN32 ffdefault.c)
target_compile_options(ffdefault PRIVATE -Wall -municode)
target_link_options(ffdefault PRIVATE -municode)
target_link_libraries(ffdefault dinput8.lib dxguid.dll)

install(TARGETS ffdefault)
