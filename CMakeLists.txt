cmake_minimum_required(VERSION 3.7)


project("cpp-parser")

add_compile_options("-fdiagnostics-color=always;-Wall;-Wpedantic;-Wextra")

include_directories(SYSTEM
    ${CLANG_PREFIX}/include
)

include_directories(src)

add_library(${PROJECT_NAME} src/parser.cpp)

find_library(LIBCLANG NAMES clang PATHS ${CLANG_PREFIX}/lib)

target_link_libraries(${PROJECT_NAME} PUBLIC ${LIBCLANG})

option(TEST "Run tests" off)
if(TEST)
    add_subdirectory(tests)
endif()
