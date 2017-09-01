
# if LLVM_PREFIX is provided
# then search LLVM from that folder
# else adding external project
if(DEFINED LLVM_PREFIX)
    # override LLVM_PREFIX to an absolute path
    get_filename_component(LLVM_PREFIX ${LLVM_PREFIX} ABSOLUTE BASE_DIR ${CMAKE_BINARY_DIR})
    set(LLVM_DIR "${LLVM_PREFIX}/lib/cmake/llvm")
else()
    execute_process(
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMAND bash ${CMAKE_SOURCE_DIR}/clang_download.sh
    )
set(LLVM_DIR "${CMAKE_BINARY_DIR}/clang-build/lib/cmake/llvm")
endif()
# directory to search for ClangConfig.cmake
set(Clang_DIR "${LLVM_DIR}/../clang")
find_package(Clang REQUIRED)

include_directories(SYSTEM
    ${LLVM_INCLUDE_DIR}
)
# code scanner library
add_library(code-scanner src/parser.cpp)
target_include_directories(code-scanner PUBLIC src)

target_link_libraries(code-scanner
    PRIVATE clang
    PRIVATE "stdc++fs"
)
