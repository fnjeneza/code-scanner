cmake_minimum_required(VERSION 3.7)


project("code-scanner")

add_compile_options("-fdiagnostics-color=always;-Wall;-Wpedantic;-Wextra")

if(NOT DEFINED LLVM_PREFIX)
    message(FATAL_ERROR "you need to provide LLVM_PREFIX path")
endif()

# override LLVM_PREFIX to an absolute path
get_filename_component(LLVM_PREFIX ${LLVM_PREFIX} ABSOLUTE BASE_DIR ${CMAKE_BINARY_DIR})

set(LLVM_INCLUDE_DIR ${LLVM_PREFIX}/include)
set(LLVM_LIB_DIR ${LLVM_PREFIX}/lib)

file(COPY ${LLVM_LIB_DIR} DESTINATION ${CMAKE_BINARY_DIR})
file(COPY "code-scanner.conf" DESTINATION ${CMAKE_BINARY_DIR}/bin)

include_directories(SYSTEM
    ${LLVM_INCLUDE_DIR}
)
add_subdirectory(src/app)

# code scanner library
add_library(${PROJECT_NAME} src/parser.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC src)

find_library(LIBCLANG NAMES clang PATHS ${LLVM_LIB_DIR})

target_link_libraries(${PROJECT_NAME}
    PUBLIC ${LIBCLANG}
    PUBLIC "stdc++fs" #PRIVATE
)

option(TEST "Run tests" off)
if(TEST)
    add_subdirectory(tests)
endif()
