add_library(cat cat.c)
target_include_directories(
  cat
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<INSTALL_INTERFACE:include>
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/cat>)

target_compile_features(cat PUBLIC c_std_99)
