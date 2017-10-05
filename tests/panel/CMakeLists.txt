cmake_minimum_required(VERSION 3.8)
project(panelTest)

find_package(glfw3 REQUIRED)
add_executable(panel_test main.cpp)

include_directories(SYSTEM ${GLEWDIR}/include ${GLMDIR}/include)

target_link_libraries(panel_test t4 t4editor glfw Glew Bearclaw2)

set(INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../../src)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/../../Bearclaw2/dependencies/glew/include)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/../../Bearclaw2/dependencies/glm/include)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/../../Bearclaw2/src)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR})
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/imgui)
set(INCLUDE_DIRS ${INCLUDE_DIRS} /usr/local/include)
 
target_include_directories(panel_test PUBLIC ${INCLUDE_DIRS})
