cmake_minimum_required(VERSION 3.3)

set(CMAKE_C_FLAGS -std=c99)

set(SRCS
    connection.c
    connection.h
    main.c
    server.c
    server.h
    service.c
    service.h)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(reverse ${SRCS})
