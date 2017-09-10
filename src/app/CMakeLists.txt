set(APP code-scanner-bin)

# code -scanner client
add_executable(${APP} src/main.cpp)
set_target_properties(${APP} PROPERTIES
    RUNTIME_OUTPUT_NAME code-scanner
    RUNTIME_OUTPUT_DIRECTORY bin
)

include_directories(
    SYSTEM ${CMAKE_SOURCE_DIR}/3rd-party/args
)
target_include_directories(${APP}
    PRIVATE ${CMAKE_SOURCE_DIR}/src/lib/include
)

target_link_libraries(${APP}
    PRIVATE ${PROJECT_NAME}
)
