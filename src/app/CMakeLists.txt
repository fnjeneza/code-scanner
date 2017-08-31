set(APP code-scanner-bin)

# code -scanner client
add_executable(${APP} src/main.cpp)
target_sources(${APP} PRIVATE src/config.cpp)
# output name
set_target_properties(${APP} PROPERTIES
    RUNTIME_OUTPUT_NAME code-scanner
    RUNTIME_OUTPUT_DIRECTORY bin
)
target_include_directories(${APP} PRIVATE ${CMAKE_SOURCE_DIR}/3rd-party/args)

target_link_libraries(${APP}
    PRIVATE ${PROJECT_NAME}
)
