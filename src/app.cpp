#include <app.h>
#include <stdio.h>
#include <stdlib.h>
#include <imgui.h>
#include <gui/panel.h>
#include <Turok4.h>
using namespace opent4;

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
        
        static const GLfloat vertex_buffer_data[] = {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
        };
        
        glGenBuffers(1, &vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);
        
        return true;
    }
    
    void application::handle_config_var(const string &name, const string &value) {
        if(name == "game_data_path") {
            m_dataPath = value;
            SetTurokDirectory(value);
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
        if(e->name == "load_level") load_level(((load_level_event*)e)->path);
        for(auto i = m_panels.begin();i != m_panels.end();i++) {
            (*i)->onEvent(e);
        }
        m_window->onEvent(e);
    }
    void application::load_level(const string &path) {
        ATRFile* atr = new ATRFile();
        atr->Load(path);
        
        Actor* act = atr->GetActors()->GetActorDef(0)->Actor->GetActor();
        
        return;
    }
    int application::run() {
        while(m_window->isOpen()) {
            m_window->poll();
            m_window->beginFrame();
            
            glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            
            //render test triangle
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
            glUseProgram(0);
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
