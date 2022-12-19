cmake_minimum_required(VERSION 3.24)
project(14_20221218_MySql_Integration_Tests C)

set(CMAKE_C_STANDARD 11)

include(FindPkgConfig)
pkg_check_modules(LIBMYSQLCLIENT REQUIRED mysqlclient)

foreach(FLAG ${LIBMYSQLCLIENT_CFLAGS_OTHER})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAG}")
endforeach()

link_directories(${LIBMYSQLCLIENT_LIBRARY_DIRS})

add_executable(14_20221218_MySql_Integration_Tests main.c)

target_include_directories(14_20221218_MySql_Integration_Tests PRIVATE ${LIBMYSQLCLIENT_INCLUDE_DIRS})
target_link_libraries(14_20221218_MySql_Integration_Tests PRIVATE ${LIBMYSQLCLIENT_LIBRARIES})