#include <render/texture.h>

namespace t4editor {
    texture::texture(int w, int h, GLenum fmt, GLenum comp_type, bool rtt, unsigned char* data) {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
		if(!data) {
			glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, comp_type, 0);
		} else {
			glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, comp_type, data);
		}
        
        if(rtt) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        }
        
        glBindTexture(GL_TEXTURE_2D, 0);
        
        m_w = w;
        m_h = h;
        m_fmt = fmt;
        m_comp_type = comp_type;
        m_isRtt = rtt;
    }

    texture::~texture() {
        if(id) glDeleteTextures(1, &id);
    }
    
    void texture::resize(int w, int h) {
        m_w = w;
        m_h = h;
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, m_fmt, m_w, m_h, 0, m_fmt, m_comp_type, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void texture::bind() {
        glBindTexture(GL_TEXTURE_2D, id);
    }
}
