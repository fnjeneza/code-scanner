
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

find_program(FLATC NAMES flatc)
if(NOT FLATC)
  message(FATAL_ERROR "flatc not found")
endif()

# generate the flatbuffers
add_custom_command(
  OUTPUT generate_idl
  COMMAND ${FLATC} --cpp ${CMAKE_CURRENT_SOURCE_DIR}/idl/database.fbs
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

get_filename_component(FLATBUFFERS_INCLUDE_DIR ${FLATC}/../../include ABSOLUTE)

# directory to search for ClangConfig.cmake
# used by find_package
set(Clang_DIR "${LLVM_DIR}/../clang")
find_package(Clang REQUIRED)

include_directories(SYSTEM
    ${LLVM_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/3rd-party/json/src
    ${CMAKE_SOURCE_DIR}/3rd-party/task_system
    ${FLATBUFFERS_INCLUDE_DIR}
)

# code scanner library
add_library(code-scanner SHARED
  src/code-scanner.cpp
  generate_idl # generate file
)

target_sources(code-scanner
    PRIVATE src/utils.cpp
    PRIVATE src/Parser_Impl.cpp
    PRIVATE src/translation_unit_t.cpp
    PRIVATE src/compile_database_t.cpp
    PRIVATE src/config.cpp
    PRIVATE src/ErrorCodes.cpp
)

target_include_directories(code-scanner
    PUBLIC include
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(code-scanner
    PRIVATE libclang
    PRIVATE "stdc++fs"
    PRIVATE pthread
)

option(TEST "Run tests" off)
if(TEST)
    add_subdirectory(test)
endif()
