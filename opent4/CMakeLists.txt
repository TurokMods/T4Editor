file(GLOB t4_top_SOURCES src/*.*)

include_directories(src/)

add_library(opent4 ${t4_top_SOURCES})

source_group("OpenT4" FILES ${t4_top_SOURCES})
