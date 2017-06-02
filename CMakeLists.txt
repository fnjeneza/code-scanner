cmake_minimum_required(VERSION 3.7)


project("cpp-parser")

include_directories(SYSTEM
    ${CLANG_PREFIX}/include
)

include_directories(src)

add_executable(${PROJECT_NAME} src/main.cpp)

target_sources(${PROJECT_NAME} PUBLIC src/parser.cpp)

find_library(LIBCLANG NAMES clang PATHS ${CLANG_PREFIX}/lib)

target_link_libraries(${PROJECT_NAME} PUBLIC ${LIBCLANG})
