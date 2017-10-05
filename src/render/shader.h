#pragma once
#include <string>
using namespace std;

#include <GLFW/glfw3.h>

namespace t4editor {
    class application;
    class shader {
        public:
            shader(application* app);
            ~shader();
        
            bool loadFromFile(const string& vert, const string& frag);
            bool loadFromMemory(const string& vert, const string& frag);
        
            void attribute(const string& name, uint32_t idx);
        
            void bind();
        
        protected:
            GLuint m_vert;
            GLuint m_frag;
            GLuint m_shader;
            application* m_app;
    };
}
