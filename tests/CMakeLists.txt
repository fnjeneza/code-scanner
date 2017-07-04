enable_testing()
set(test_name "test_parser")

add_executable(${test_name} test_parser.cpp)
target_include_directories(${test_name} PRIVATE
    ${CMAKE_SOURCE_DIR}/3rd-party/Catch/single_include)

target_link_libraries(${test_name}
    ${PROJECT_NAME}
    "stdc++fs" #experimental filesystem
)

file(COPY data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
add_custom_command(OUTPUT export_compile_commands
    COMMAND ${CMAKE_COMMAND} -DCMAKE_EXPORT_COMPILE_COMMANDS=on .
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/data
)

add_test(${test_name} ${test_name})
add_custom_target(check
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND ${test_name}
    DEPENDS export_compile_commands
)
