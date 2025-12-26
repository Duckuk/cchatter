cmake_minimum_required(VERSION 4.1.2)

project(Networking
    VERSION 1.0.0
    LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(DEFAULT_FLAGS "-Weverything -Wno-switch-enum -Wno-declaration-after-statement -Wno-jump-misses-init -Wno-unsafe-buffer-usage")

# Library
add_library(network)
target_sources(network
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
target_compile_options(network
    PRIVATE
        ${DEFAULT_FLAGS})

# Binaries
add_executable(client)
target_sources(client
    PRIVATE
        src/client/main.c)
target_compile_options(client
    PRIVATE
        ${DEFAULT_FLAGS})

add_executable(server)
target_sources(server
    PRIVATE
        src/server/main.c)
target_compile_options(server
    PRIVATE
        ${DEFAULT_FLAGS})


target_link_libraries(client
    PRIVATE
        network
)
target_link_libraries(server
    PRIVATE
        network
)

# Tests
enable_testing()
add_executable(vec_test tests/vec.c)
target_link_libraries(vec_test
    PRIVATE
        network)
add_test(NAME vec_test COMMAND vec_test)