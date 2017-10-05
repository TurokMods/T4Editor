cmake_minimum_required(VERSION 3.8)
project(t4editor)

find_package(glfw3 REQUIRED)

add_subdirectory(opent4)
add_subdirectory(Bearclaw2)
add_subdirectory(editor)
add_subdirectory(tests)

file(GLOB editor_src
    "src/*.h"
    "src/*.cpp"
)

file(GLOB gui_src
    "src/gui/*.h"
    "src/gui/*.cpp"
)

file(GLOB turokfs_src
    "src/turokfs/*.h"
    "src/turokfs/*.cpp"
)

file(GLOB imgui_src
    "deps/imgui/*.h"
    "deps/imgui/*cpp"
)

source_group(src FILES ${editor_src})
source_group(src\\turokfs FILES ${turokfs_src})
source_group(src\\gui FILES ${gui_src})
source_group(deps\\imgui FILES ${imgui_src})

add_library(t4editor ${editor_src} ${turokfs_src} ${gui_src} ${imgui_src})

include_directories(SYSTEM ${GLEWDIR}/include ${GLMDIR}/include)

target_link_libraries(t4editor opent4 glfw Glew Bearclaw2)

set(INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/Bearclaw2/dependencies/glew/include)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/Bearclaw2/dependencies/glm/include)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/Bearclaw2/src)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/opent4/src)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR})
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/deps/imgui)
set(INCLUDE_DIRS ${INCLUDE_DIRS} /usr/local/include)
 
target_include_directories(t4editor PUBLIC ${INCLUDE_DIRS})
