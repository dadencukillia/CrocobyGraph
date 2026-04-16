add_library(incbin "${CMAKE_SOURCE_DIR}/external/incbin/incbin.c")
target_include_directories(incbin PUBLIC "${CMAKE_SOURCE_DIR}/external/incbin")

set(incbin_FOUND true)
