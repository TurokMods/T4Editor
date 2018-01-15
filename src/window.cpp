#include <window.h>
#include <stdio.h>
#include <imgui.h>
#include <gui/imgui_setup.h>
#include <gui/panel.h>
#include <app.h>
#include <logger.h>

namespace t4editor {
    void window_size_callback(GLFWwindow* window, int width, int height) {
        int x, y;
        glfwGetFramebufferSize(window, &x, &y);
        app_resize_event e(x, y, window);
        application* app = (application*)glfwGetWindowUserPointer(window);
        app->onEvent(&e);
    }
    window::window(application* app, int w, int h) {
        m_app = app;
        if(!glfwInit()) {
            printf("Failed to initialize GLFW\n");
        }
        
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_DEPTH_BITS,24);
        
        m_window = glfwCreateWindow(w, h, "Turok4 Editor", 0, 0);
        if(!m_window) {
            glfwTerminate();
            printf("Failed to create GLFW window\n");
            m_window = 0;
        } else {
            glfwMakeContextCurrent(m_window);
            glfwSetWindowUserPointer(m_window,m_app);
            glfwSetWindowSizeCallback(m_window,window_size_callback);
            
            glewExperimental = GL_TRUE;
            if (glewInit() != GLEW_OK) {
                printf("Couldn't initialize GLEW library.\n");
                glfwTerminate();
                m_window = 0;
                return;
            }
            
            GLint dbits, abits;
            glGetIntegerv(GL_DEPTH_BITS, &dbits);
            glGetIntegerv(GL_ALPHA_BITS, &abits);
            printf("GL Version: %s\n", glGetString(GL_VERSION));
            printf("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
            printf("Vendor: %s\n", glGetString(GL_VENDOR));
            printf("Renderer: %s\n", glGetString(GL_RENDERER));
            printf("Depth bits: %d\n", dbits);
            printf("Alpha bits: %d\n", abits);
            
            ImGui_ImplGlfwGL3_Init(m_window, true);
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
    window::~window() {
        if(m_window) {
            glfwTerminate();
            ImGui_ImplGlfwGL3_Shutdown();
        }
    }
    
    void window::onEvent(event *e) {
        if(e->name == "shutdown") {
            glfwSetWindowShouldClose(m_window,true);
        }
    }
    bool window::isOpen() const {
        if(!m_window) return false;
        return !glfwWindowShouldClose(m_window);
    }
    void window::beginFrame() {
        ImGui_ImplGlfwGL3_NewFrame();
    }
    void window::endFrame() {        
        ImGui::Render();
        glfwSwapBuffers(m_window);
    }
    void window::poll() {
        glfwPollEvents();
    }
    vec2 window::getSize(bool UseBufferSize) const {
        int x, y;
        if(UseBufferSize)
            glfwGetFramebufferSize(m_window, &x, &y);
        else
            glfwGetWindowSize(m_window, &x, &y);
        return vec2(x, y);
    }
    vec2 window::getPosition() const {
        int x, y;
        glfwGetWindowPos(m_window, &x, &y);
        return vec2(x, y);
    }
}
