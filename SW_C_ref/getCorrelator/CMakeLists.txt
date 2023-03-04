cmake_minimum_required(VERSION 3.23)
project(tracking_c99 C)

set(CMAKE_C_STANDARD 99)

add_executable(tracking_c99 main.c tracking.c tracking.h)
