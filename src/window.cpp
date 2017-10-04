#include <window.h>
#include <stdio.h>

namespace t4editor {
    window::window(int w, int h) {
        if(!glfwInit()) {
            printf("Failed to initialize GLFW\n");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        
        m_window = glfwCreateWindow(w, h, "Turok4 Editor", 0, 0);
        if(!m_window) {
            printf("Failed to create GLFW window\n");
        }
    }
    window::~window() {
        if(m_window) glfwTerminate();
    }

    bool window::isOpen() const {
        return glfwWindowShouldClose(m_window);
    }
}
