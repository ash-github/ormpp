cmake_minimum_required(VERSION 2.8)
project(ormpp)

set(CMAKE_CXX_STANDARD 14 -pthread)
set(SOURCE_FILES
        main.cpp
        sqlite3.c
        )
add_executable(ormpp ${SOURCE_FILES})
#target_link_libraries(untitled3 dl)