cmake_minimum_required(VERSION 3.25)
project(CSE474)

set(CMAKE_CXX_STANDARD 14)

include_directories("Homeworks/C Programming 2/c_prog2_arduino")

add_executable(CSE474
        "Homeworks/C Progamming 1/c_asgn01_empty.c"
        "Homeworks/C Programming 2/c_prog2_arduino/c_prog2.c"
        "Homeworks/C Programming 2/c_prog2_arduino/c_prog2.h"
        "Homeworks/C Programming 2/c_prog2_arduino/c_prog2_arduino.ino")
