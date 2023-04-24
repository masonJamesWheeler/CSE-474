cmake_minimum_required(VERSION 3.25)
project(CSE474)

set(CMAKE_CXX_STANDARD 14)

include_directories("Homeworks/C_Programming02/c_prog2_arduino")

add_executable(CSE474
        "Homeworks/C_Progamming01/c_asgn01_empty.c"
        "Homeworks/C_Programming02/c_prog2_arduino/c_prog2.c"
        "Homeworks/C_Programming02/c_prog2_arduino/c_prog2.h"
        "Homeworks/C_Programming02/c_prog2_arduino/c_prog2_arduino.ino")
