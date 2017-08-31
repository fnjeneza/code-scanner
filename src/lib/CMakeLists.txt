
# code scanner library
add_library(${PROJECT_NAME} src/parser.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC src)

find_library(LIBCLANG NAMES clang PATHS ${LLVM_LIB_DIR})

target_link_libraries(${PROJECT_NAME}
    PUBLIC ${LIBCLANG}
    PUBLIC "stdc++fs" #PRIVATE
)
