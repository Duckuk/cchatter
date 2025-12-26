cmake_minimum_required(VERSION 4.1.2)

project(cchatter
    VERSION 1.0.0
    LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(DEFAULT_FLAGS "-Wall" "-Wextra")

# Library
add_library(cchat)
target_sources(cchat
    PRIVATE
        include/vec.c
        include/packet.c
        include/socket.c
    
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
        FILES
            include/vec.h
            include/packet.h
            include/socket.h
)
target_compile_options(cchat
    PRIVATE
        ${DEFAULT_FLAGS})

# Binaries
add_executable(cchat-client)
target_sources(cchat-client
    PRIVATE
        src/client/main.c)
target_compile_options(cchat-client
    PRIVATE
        ${DEFAULT_FLAGS})

add_executable(cchat-server)
target_sources(cchat-server
    PRIVATE
        src/server/main.c)
target_compile_options(cchat-server
    PRIVATE
        ${DEFAULT_FLAGS})


target_link_libraries(cchat-client
    PRIVATE
        cchat
)
target_link_libraries(cchat-server
    PRIVATE
        cchat
)

# Tests
enable_testing()
add_executable(vec_test tests/vec.c)
target_link_libraries(vec_test
    PRIVATE
        cchat)
add_test(NAME vec_test COMMAND vec_test)