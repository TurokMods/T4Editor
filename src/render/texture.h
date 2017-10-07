#include <GLFW/glfw3.h>

namespace t4editor {
    class texture {
        public:
            texture(int w, int h, GLenum fmt, GLenum comp_type, bool rtt = false);
            ~texture();
        
            void bind();
        
            GLuint id;
    };
};
