#pragma once
#include <vector>
using namespace std;

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace t4editor {
    class ui_panel;
    class application;
    class window {
        public:
            window(application* app, int width, int height);
            ~window();
        
            void add_panel(ui_panel* panel);
            void remove_panel(ui_panel* panel);

            bool isOpen() const;
            void beginFrame();
            void endFrame();
            void poll();
            
        protected:
            vector<ui_panel*> m_panels;
            GLFWwindow* m_window;
    };
}
