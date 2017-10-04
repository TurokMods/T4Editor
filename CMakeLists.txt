cmake_minimum_required(VERSION 3.9)
project(t4editor)

find_package(glfw3 REQUIRED)

add_subdirectory(T4Loader)
add_subdirectory(Bearclaw2)

file(GLOB editor_src
    "src/*.h"
    "src/*.cpp"
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
source_group(deps\\imgui FILES ${imgui_src})

add_executable(editor ${editor_src} ${turokfs_src} ${imgui_src})
target_link_libraries(editor t4 glfw Glew Bearclaw2)

set(INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/Bearclaw2/dependencies/glew)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/Bearclaw2/dependencies/glm)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/Bearclaw2/src)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR})
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/deps/imgui)
set(INCLUDE_DIRS ${INCLUDE_DIRS} /usr/local/include)
 
target_include_directories(editor PUBLIC ${INCLUDE_DIRS})
