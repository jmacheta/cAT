cmake_minimum_required(VERSION 3.0)

add_executable(cat_example_demo demo.c)
target_link_libraries(cat_example_demo cat)

add_executable(cat_example_basic basic.c)
target_link_libraries(cat_example_basic cat)

add_executable(cat_example_unsolicited unsolicited.c)
target_link_libraries(cat_example_unsolicited cat)
