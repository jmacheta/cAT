cmake_minimum_required(VERSION 3.0)

add_executable(demo demo.c)
target_link_libraries(demo cat)

add_executable(basic basic.c)
target_link_libraries(basic cat)

add_executable(unsolicited unsolicited.c)
target_link_libraries(unsolicited cat)
