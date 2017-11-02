enable_testing()
set(test_name "code_parser_test")

add_executable(${test_name} main.cpp)
target_include_directories(${test_name}
    PRIVATE ${CMAKE_SOURCE_DIR}/3rd-party/Catch/single_include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include
    )

target_link_libraries(${test_name}
    ${PROJECT_NAME}
    "stdc++fs" #experimental filesystem
)

file(COPY compile_commands.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_test(${test_name} ${test_name})
add_custom_target(check
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND ${test_name}
)
