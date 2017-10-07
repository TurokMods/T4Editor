#include <render/texture.h>

namespace t4editor {
    texture::texture(int w, int h, GLenum fmt, GLenum comp_type, bool rtt) {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, comp_type, 0);
        
        if(rtt) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        }
        
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    texture::~texture() {
        if(id) glDeleteTextures(1, &id);
    }
    
    void texture::bind() {
        glBindTexture(GL_TEXTURE_2D, id);
    }
}
