cmake_minimum_required(VERSION 3.7)

project("code-scanner")

add_compile_options(
    "-std=c++1z"
    "-fdiagnostics-color=always;-Wall;-Wpedantic;-Wextra"
    "-Winline;-Wshadow;-Wuseless-cast;-Wsign-conversion;-Wctor-dtor-privacy"
    "-Wmissing-include-dirs;-Wduplicated-cond;-Wfloat-equal;-Wlogical-op"
    "-Wmissing-declarations;-Wsuggest-override;-Wconversion;-Wredundant-decls"
    "-Wnon-virtual-dtor;-Wold-style-cast;-Wcast-align;-Woverloaded-virtual"
    #"-Weffc++"
)

add_subdirectory(src/lib)
add_subdirectory(src/app)

option(TEST "Run tests" off)
if(TEST)
    add_subdirectory(tests)
endif()
