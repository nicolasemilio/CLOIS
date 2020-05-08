cmake_minimum_required(VERSION 3.12)
project(EVOCA)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(
        globales.cpp
        globales.h
        main.cpp includes.h)
