cmake_minimum_required(VERSION 3.8)
project(t4editor)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/lib)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake-extras/")

add_subdirectory(deps)

ADD_DEFINITIONS(
    -std=c++11 # Or -std=c++0x
    # Other flags
)

add_subdirectory(opent4)

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

file(GLOB render_src
    "src/render/*.h"
    "src/render/*.cpp"
)

file(GLOB imgui_src
    "deps/imgui/*.h"
    "deps/imgui/*cpp"
)

file(GLOB soil_src
    "src/render/SOIL/*.h"
    "src/render/SOIL/*.cpp"
)

file(GLOB util_src
    "src/util/*.h"
    "src/util/*.hpp"
    "src/util/*.cpp"
)

source_group(src FILES ${editor_src})
source_group(src\\turokfs FILES ${turokfs_src})
source_group(src\\gui FILES ${gui_src})
source_group(src\\util FILES ${util_src})
source_group(src\\render FILES ${render_src})
source_group(deps\\imgui FILES ${imgui_src})
source_group(src\\render\\soil FILES ${soil_src})

file(GLOB fonts_src "data/fonts/*.*")
file(GLOB shaders_src "data/shaders/*.*")
source_group(data\\fonts FILES ${fonts_src})
source_group(data\\shaders FILES ${shaders_src})

add_executable(t4editor ${editor_src} ${turokfs_src} ${gui_src} ${imgui_src} ${render_src} ${fonts_src} ${shaders_src} ${soil_src} ${util_src})

include_directories(SYSTEM ${GLEWDIR}/include ${GLMDIR}/include ${GLFWDIR}/include)

target_link_libraries(t4editor opent4 Glfw Glew ${GLFW_LIBRARIES})

set(INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/deps/glew/include)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/deps/glm/include)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw/include)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/opent4/src)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR})
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/deps/imgui)
set(INCLUDE_DIRS ${INCLUDE_DIRS} /usr/local/include)
 
target_include_directories(t4editor PUBLIC ${INCLUDE_DIRS})
