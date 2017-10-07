#include <render/framebuffer.h>
#include <render/texture.h>

namespace t4editor {
    framebuffer::framebuffer(int _w, int _h, bool depth) {
        glGenFramebuffers(1, &fbo);\
        w = _w;
        h = _h;
        m_depth = depth;
        
        if(depth) {
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
            glGenRenderbuffers(1, &dbo);
            glBindRenderbuffer(GL_RENDERBUFFER, dbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, dbo);
            
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
        } else dbo = 0;
    }
    framebuffer::~framebuffer() {
        glDeleteFramebuffers(1, &fbo);
        if(m_depth) glDeleteRenderbuffers(1, &dbo);
    }
    
    void framebuffer::bind() {
        if(w != m_lastWidth || h != m_lastHeight) {
            // not sure if all of this is necessary, TODO: see if all this is necessary
            glDeleteFramebuffers(1, &fbo);
            glGenFramebuffers(1, &fbo);
            
            if(m_depth) {
                glDeleteRenderbuffers(1, &dbo);
                glBindFramebuffer(GL_FRAMEBUFFER, fbo);
                glGenRenderbuffers(1, &dbo);
                glBindRenderbuffer(GL_RENDERBUFFER, dbo);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, dbo);
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glBindRenderbuffer(GL_RENDERBUFFER, dbo);
        
        vector<GLenum> buffers;
        for(size_t i = 0;i < attachments.size();i++) {
            buffers.push_back(GL_COLOR_ATTACHMENT0 + i);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, attachments[i]->id, 0);
        }
        glDrawBuffers(buffers.size(), &buffers[0]);
        
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            printf("Warning: Framebuffer incomplete for some reason\n");
        }
        
        glViewport(0, 0, w, h);
        m_lastWidth = w;
        m_lastHeight = h;
    }
    
    void framebuffer::blit(GLenum attachmentId) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
        glReadBuffer(attachmentId);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0 ,0 ,w ,h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }
}
