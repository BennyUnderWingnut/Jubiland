cmake_minimum_required(VERSION 3.12)
project(client C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lcurses -lpanel -lprotobuf-c -ggdb")

add_executable(client enums.pb-c.c enums.pb-c.h request.pb-c.c request.pb-c.h response.pb-c.c response.pb-c.h
        map.c map.h client.c login.c login.h socklib.c socklib.h character.c character.h mylib.c mylib.h
        sync.c sync.h game.c game.h creature.c creature.h interface.c interface.h)