cmake_minimum_required(VERSION 3.7)

project("code-scanner")

add_compile_options("-fdiagnostics-color=always;-Wall;-Wpedantic;-Wextra")

add_subdirectory(src/lib)
add_subdirectory(src/app)

option(TEST "Run tests" off)
if(TEST)
    add_subdirectory(tests)
endif()
