cmake_minimum_required(VERSION 3.9)
project(t4editor)

add_subdirectory(T4Loader)
add_subdirectory(Bearclaw2)

file(GLOB editor_src
    "src/*.h"
    "src/*.cpp"
)

add_executable(editor ${editor_src})
target_link_libraries(editor t4 glfw Glew Bearclaw2)
target_include_directories(editor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src)
