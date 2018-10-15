cmake_minimum_required(VERSION 3.12)
project(server C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lpthread -lprotobuf-c -g -v -da -Q")


add_executable(server request.pb-c.c request.pb-c.h response.pb-c.c response.pb-c.h enums.pb-c.c enums.pb-c.h
        server.c server.h connection.c connection.h socklib.c socklib.h thread_pool.c thread_pool.h game.c game.h
        character.c character.h thpool.c thpool.h mylib.c mylib.h map.c map.h creature.c creature.h event.c event.h
        ai.c ai.h)