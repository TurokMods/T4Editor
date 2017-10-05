#include <app.h>
#include <stdio.h>
#include <stdlib.h>
#include <imgui.h>
#include <gui/panel.h>

#include <cassert>

void logShaderError(GLuint ShaderID) {
    GLint status = 0;
    glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &status);
    if(status == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &maxLength);
        
        char errorLog[maxLength];
        glGetShaderInfoLog(ShaderID, maxLength, &maxLength, &errorLog[0]);
        glDeleteShader(ShaderID);
        printf("%s", errorLog);
    }
}

namespace t4editor {
    application::application(int argc, const char* argv[]) {
        for(int i = 0;i < argc;i++) {
            m_args.push_back(string(argv[i]));
        }
        m_windowWidth = 1200;
        m_windowHeight = 900;
        m_window = 0;
        m_fs = 0;
    }
    application::~application() {
        if(m_window) delete m_window;
        if(m_fs) delete m_fs;
    }
    
    bool application::initialize() {
        if(!load_config()) return false;
        m_window = new window(this, m_windowWidth, m_windowHeight);
        if(!m_window->isOpen()) {
            return false;
        }
        
        m_fs = new turokfs(m_dataPath);
        
        // Setup test openGL triangle
        glGenVertexArrays(1, &VertexArrayID);
        glBindVertexArray(VertexArrayID);
        
        static const GLfloat vertices[] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f, 0.5f, 0.0f,
        };
        
        glGenBuffers(1, &VertexBufferID);
        glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        
        GLuint VertexID = glCreateShader(GL_VERTEX_SHADER);
        GLuint FragmentID = glCreateShader(GL_FRAGMENT_SHADER);
        
        const char* VertexSource = "\
        #version 150 core\
        in vec3 position;\
        void main() { gl_Position = vec4(position, 1.0); }";
        
        glShaderSource(VertexID, 1, &VertexSource, NULL);
        glCompileShader(VertexID);
        logShaderError(VertexID);
        
        const char* FragmentSource = "\
        #version 150 core\
        out vec4 outcolor;\
        void main() { outcolor = vec4(0, 1, 1, 1); }";
        glShaderSource(FragmentID, 1, &FragmentSource, NULL);
        glCompileShader(FragmentID);
        logShaderError(FragmentID);
        
        ShaderID = glCreateProgram();
        glAttachShader(ShaderID, VertexID);
        glAttachShader(ShaderID, FragmentID);
        glLinkProgram(ShaderID);
        
        return true;
    }
    
    void application::handle_config_var(const string &name, const string &value) {
        if(name == "game_data_path") {
            m_dataPath = value;
            printf("Using '%s' as game data directory\n", m_dataPath.c_str());
        }
        else if(name == "editor_data_path") {
            m_editorDataPath = value;
            printf("Using %s as editor data directory\n", m_editorDataPath.c_str());
        }
        else if(name == "res_x") m_windowWidth = atoi(value.c_str());
        else if(name == "res_y") m_windowHeight = atoi(value.c_str());
    }

    
    void application::add_panel(ui_panel *panel) {
        m_panels.push_back(panel);
        panel->m_app = this;
        panel->onAttach(nullptr);
    }
    
    void application::remove_panel(ui_panel *panel) {
        for(auto i = m_panels.begin();i != m_panels.end();i++) {
            if((*i) == panel) {
                m_panels.erase(i);
                panel->m_app = nullptr;
                return;
            }
        }
    }
    
    void application::onEvent(event *e) {
        if(e->name == "load_level") {
        }
        for(auto i = m_panels.begin();i != m_panels.end();i++) {
            (*i)->onEvent(e);
        }
        m_window->onEvent(e);
    }
    void application::load_level(const string &path) {
        
    }
    int application::run() {
        while(m_window->isOpen()) {
            m_window->poll();
            m_window->beginFrame();
            
            glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            
            //render test triangle
            glUseProgram(ShaderID);
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glDisableVertexAttribArray(0);
            
            //render ImGui windows
            for(auto i = m_panels.begin();i != m_panels.end();i++) {
                (*i)->render();
            }
            
            m_window->endFrame();
        }
        return 0;
    }
}
