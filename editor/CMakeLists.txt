cmake_minimum_required(VERSION 3.8)
project(editorApp)

find_package(glfw3 REQUIRED)

file(GLOB ui_src
    "ui/*.h"
    "ui/*.cpp"
)

source_group(ui FILES ${ui_src})

add_executable(editorApp main.cpp ${ui_src})

include_directories(SYSTEM ${GLEWDIR}/include ${GLMDIR}/include)

target_link_libraries(editorApp t4editor t4 glfw Glew Bearclaw2)

set(INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/../src)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/../Bearclaw2/dependencies/glew/include)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/../Bearclaw2/dependencies/glm/include)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/../Bearclaw2/src)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR})
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/../deps/imgui)
set(INCLUDE_DIRS ${INCLUDE_DIRS} /usr/local/include)
 
target_include_directories(editorApp PUBLIC ${INCLUDE_DIRS})
