
# if LLVM_PREFIX is provided
# then search LLVM from that folder
# else adding external project
if(DEFINED LLVM_PREFIX)
    # override LLVM_PREFIX to an absolute path
    get_filename_component(LLVM_PREFIX ${LLVM_PREFIX} ABSOLUTE BASE_DIR ${CMAKE_BINARY_DIR})
    set(LLVM_DIR "${LLVM_PREFIX}/lib/cmake/llvm")
else()
    # download and compile clang
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
    ${CMAKE_SOURCE_DIR}/3rd-party/json/src
    ${CMAKE_SOURCE_DIR}/3rd-party/flatbuffers/include
)

# code scanner library
add_library(code-scanner src/code-scanner.cpp)

target_sources(code-scanner
    PRIVATE src/utils.cpp
    PRIVATE src/Parser_Impl.cpp
    PRIVATE src/translation_unit_t.cpp
    PRIVATE src/compile_database_t.cpp
    PRIVATE src/config.cpp
)

target_include_directories(code-scanner
    PUBLIC include
)

target_link_libraries(code-scanner
    PRIVATE libclang
)
