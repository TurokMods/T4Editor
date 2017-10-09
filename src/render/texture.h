#ifndef T_TEXTURE_H
#define T_TEXTURE_H

#include <EngineTypes.h>
#include <GLFW/glfw3.h>

namespace t4editor {
    class texture {
        public:
            texture(int w, int h, GLenum fmt, GLenum comp_type, bool rtt = false, unsigned char* data = 0);
            ~texture();
        
            void resize(int w, int h);
            void bind();
        
            GLuint id;
        protected:
            int m_w;
            int m_h;
            GLenum m_fmt;
            GLenum m_comp_type;
            bool m_isRtt;
    };
};


#endif