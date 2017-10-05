#include <app.h>
#include <stdio.h>
#include <stdlib.h>
#include <imgui.h>
#include <gui/panel.h>

#include <cassert>



namespace t4editor {
    application::application(int argc, const char* argv[]) {
        for(int i = 0;i < argc;i++) {
            m_args.push_back(string(argv[i]));
        }
        m_windowWidth = 1200;
        m_windowHeight = 900;
        m_window = 0;
        m_fs = 0;
        m_level = 0;
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
        
        m_shader = new shader(this);
        m_shader->loadFromFile("shaders/simple.vsh", "shaders/simple.fsh");
        m_shader->attribute("position", 0);
        m_shader->attribute("normal", 1);
        m_shader->attribute("texc", 2);
        
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
        m_level = new level();
        
        if(!m_level->load(path)) {
            delete m_level;
            m_level = 0;
        }
        
        return;
    }
    
    int application::run() {
        while(m_window->isOpen()) {
            m_window->poll();
            m_window->beginFrame();
            
            glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            
            m_shader->bind();
            
            if(m_level) {
                vector<actor*> actors = m_level->actors();
                for(int i = 0;i < actors.size();i++) {
                    actor* a = actors[i];
                    if(a->meshes.size() > 0) {
                        for(int j = 0;j < a->meshes.size();j++) {
                            actor_mesh* m = a->meshes[j];
                            m->render();
                        }
                    }
                }
            }
            
            //render ImGui windows
            for(auto i = m_panels.begin();i != m_panels.end();i++) {
                (*i)->render();
            }
            
            m_window->endFrame();
        }
        return 0;
    }
}
