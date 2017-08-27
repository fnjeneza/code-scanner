cmake_minimum_required(VERSION 3.7)


project("cpp-parser")
set(CMAKE_CURRENT_BINARY_DIR ${CMAKE_BINARY_DIR}/bin)

add_compile_options("-fdiagnostics-color=always;-Wall;-Wpedantic;-Wextra")

if(NOT DEFINED LLVM_PREFIX)
    message(FATAL_ERROR "you need to provide LLVM_PREFIX path")
endif()
set(LLVM_INCLUDE_DIR ${CMAKE_BINARY_DIR}/${LLVM_PREFIX}/include)
set(LLVM_LIB_DIR ${CMAKE_BINARY_DIR}/${LLVM_PREFIX}/lib)
message(STATUS ${LLVM_LIB_DIR})

include_directories(SYSTEM
    ${LLVM_INCLUDE_DIR}
)

include_directories(src)

add_library(${PROJECT_NAME} src/parser.cpp)

find_library(LIBCLANG NAMES clang PATHS ${LLVM_LIB_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC ${LIBCLANG})

option(TEST "Run tests" off)
if(TEST)
    add_subdirectory(tests)
endif()
