#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
using namespace std;

#include <glm/glm.hpp>
using namespace glm;

namespace t4editor {
    class texture;
    class framebuffer {
        public:
            framebuffer(int w, int h, bool depth);
            ~framebuffer();
        
            void bind();
            void resize(int w, int h, bool doBind = true);
            void clear(const vec4& color = vec4(0.5f, 0.5f, 0.5f, 1.0f), float depth = 1.0f);
        
            void blit(GLenum attachmentId = GL_COLOR_ATTACHMENT0, int dx = 0, int dy = 0);
        
            vector<texture*> attachments;
            GLuint fbo;
            GLuint dbo;
            int w;
            int h;
        
        protected:
            int m_lastWidth;
            int m_lastHeight;
            vec4 m_lastClearColor;
            float m_lastClearDepth;
            bool m_depth;
    };
}
