cmake_minimum_required(VERSION 3.0)

project(libcat LANGUAGES C)

option(CAT_BUILD_TESTS "Force build tests" OFF)
option(CAT_BUILD_EXAMPLES "build examples" OFF)

add_subdirectory(src)

# if(PROJECT_IS_TOP_LEVEL OR CAT_BUILD_TESTS) enable_testing()
# add_subdirectory(tests) endif()

# if(PROJECT_IS_TOP_LEVEL OR CAT_BUILD_EXAMPLES) add_subdirectory(example)
# endif()
