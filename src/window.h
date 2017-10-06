#pragma once
#include <glm/glm.hpp>
using namespace glm;

#include <vector>
using namespace std;

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <event.h>

namespace t4editor {
    class ui_panel;
    class application;
    class window : public event_receiver {
        public:
            window(application* app, int width, int height);
            ~window();
        
            virtual void onEvent(event* e);

            bool isOpen() const;
            void beginFrame();
            void endFrame();
            void poll();
        
            vec2 getSize() const;
        
            GLFWwindow* getWindow() const { return m_window; }
            
        protected:
            GLFWwindow* m_window;
            application* m_app;
    };
}
