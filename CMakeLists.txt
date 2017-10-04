cmake_minimum_required(VERSION 3.9)
project(t4editor)

find_package(glfw3 REQUIRED)

add_subdirectory(T4Loader)
add_subdirectory(Bearclaw2)

file(GLOB editor_src
    "src/*.h"
    "src/*.cpp"
)

source_group(src FILES ${editor_src})

add_executable(editor ${editor_src})
target_link_libraries(editor t4 glfw Glew Bearclaw2)

set(GLEWDIR    "${CMAKE_CURRENT_SOURCE_DIR}/Bearclaw2/dependencies/glew"     )
set(GLMDIR     "${CMAKE_CURRENT_SOURCE_DIR}/Bearclaw2/dependencies/glm"      )

target_include_directories(editor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/Bearclaw2/src ${OPENGL_INCLUDE_DIR} ${GLEWDIR}/include ${GLMDIR}/include /usr/local/include)
