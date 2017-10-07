#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
using namespace std;

namespace t4editor {
    class texture;
    class framebuffer {
        public:
            framebuffer(int w, int h, bool depth);
            ~framebuffer();
        
            void bind();
        
            void blit(GLenum attachmentId = GL_COLOR_ATTACHMENT0);
        
            vector<texture*> attachments;
            GLuint fbo;
            GLuint dbo;
            int w;
            int h;
        
        protected:
            int m_lastWidth;
            int m_lastHeight;
            bool m_depth;
    };
}
