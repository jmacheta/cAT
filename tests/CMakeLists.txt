cmake_minimum_required(VERSION 3.0)

target_compile_options(
  cat
  PUBLIC -Wall
         -Wextra
         -Wunused-parameter
         -Winit-self
         -Wcast-align
         -Wconversion
         -Wpedantic
         -Wnull-dereference
         -Wduplicated-cond
         -Wnull-dereference
         -Wsign-conversion
         -Wlogical-op
         -Wdouble-promotion
         -Wno-missing-field-initializers)

cmake_policy(SET CMP0135 NEW)

include(FetchContent)
include(GoogleTest)

FetchContent_Declare(
  googletest URL https://github.com/google/googletest/archive/main.zip
                 FIND_PACKAGE_ARGS NAMES GTest)

enable_language(CXX)

FetchContent_MakeAvailable(googletest)

add_executable(
  cat_ut
  test_run.cpp
  test_parse.cpp
  test_cmd_list.cpp
  test_write_hex_range.cpp
  test_hold_state.cpp
  test_implicit_write.cpp
  test_mutex.cpp
  test_order.cpp
  test_read_args.cpp
  test_read.cpp
  test_return_read.cpp
  test_return_run.cpp
  test_return_test.cpp
  test_return_write.cpp
  test_search_cmd.cpp
  test_shortcuts.cpp
  test_test_args.cpp
  test_test_only.cpp
  test_test.cpp
  test_unsolicited_read_buffer.cpp
  test_unsolicited_read_stress.cpp
  test_unsolicited_read.cpp
  test_unsolicited_test.cpp
  test_var_access.cpp
  test_write_hex_buffer.cpp
  test_write_int_range.cpp
  test_write_parse.cpp
  test_write_string_buffer.cpp
  test_write_uint_range.cpp
  test_write.cpp)

target_compile_features(cat_ut PRIVATE cxx_std_20)

target_link_libraries(cat_ut PRIVATE cat gtest_main)
gtest_discover_tests(cat_ut)

# Exclude googletest from all to make use of install target
if(IS_DIRECTORY "${googletest_SOURCE_DIR}")
  set_property(DIRECTORY ${googletest_SOURCE_DIR} PROPERTY EXCLUDE_FROM_ALL YES)
endif()
