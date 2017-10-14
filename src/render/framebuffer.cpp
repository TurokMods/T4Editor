#include <render/framebuffer.h>
#include <render/texture.h>
#include <logger.h>

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
        
        m_lastClearColor = vec4(0, 0, 0, 1);
        m_lastClearDepth = 1.0f;
    }
    framebuffer::~framebuffer() {
        glDeleteFramebuffers(1, &fbo);
        if(m_depth) glDeleteRenderbuffers(1, &dbo);
    }
    
    void framebuffer::bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        if(m_depth) glBindRenderbuffer(GL_RENDERBUFFER, dbo);
        
        // resize textures / depth buffer if necessary
        if(w != m_lastWidth || h != m_lastHeight) resize(w, h, false);
        
        // tell opengl where to render
        vector<GLenum> buffers;
        for(size_t i = 0;i < attachments.size();i++) {
			if(attachments[i]->getWidth() != w || attachments[i]->getHeight() != h) attachments[i]->resize(w, h);
            buffers.push_back(GL_COLOR_ATTACHMENT0 + i);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, attachments[i]->id, 0);
        }
        glDrawBuffers(buffers.size(), &buffers[0]);
        
        // make sure it's good
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            printf("Warning: Framebuffer incomplete for some reason\n");
        }
        
        glViewport(0, 0, w, h);
    }
    
    void framebuffer::resize(int _w, int _h, bool doBind) {
        //if(_w == w && _h == h) return;
        
        if(doBind) {
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
            if(m_depth) glBindRenderbuffer(GL_RENDERBUFFER, dbo);
        }
        if(m_depth) glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
            
        for(size_t i = 0;i < attachments.size();i++) {
            attachments[i]->resize(w, h);
        }
        
        clear(m_lastClearColor, m_lastClearDepth);
        
        if(doBind) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            if(m_depth) glBindRenderbuffer(GL_RENDERBUFFER, 0);
        }
        
        w = _w;
        h = _h;
        m_lastWidth = _w;
        m_lastHeight = _w;
    }
    
    void framebuffer::clear(const vec4& color, float depth) {
        glClearColor(color.x, color.y, color.z, color.w);
        glClearDepth(depth);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_lastClearColor = color;
        m_lastClearDepth = depth;
    }
    
    void framebuffer::blit(GLenum attachmentId, int dx, int dy) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
        glReadBuffer(attachmentId);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0 ,0 ,w ,h, dx, dy, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }
}
