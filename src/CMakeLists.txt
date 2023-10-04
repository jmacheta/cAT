add_library(cat cat.c)
target_include_directories(
  cat
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<INSTALL_INTERFACE:include>
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/cat>)

# target_compile_options( cat PRIVATE -Wall -Wextra -Wformat=2
# -Wformat-overflow=2 -Wformat-security -Wformat-signedness -Wunused-parameter
# -Winit-self -Wcast-align -Wconversion -Wpedantic -Wnull-dereference
# -Wduplicated-cond -Wnull-dereference -Wsign-conversion -Wlogical-op
# -Wdouble-promotion)
