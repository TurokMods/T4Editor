#include <app.h>
#include <stdio.h>
#include <stdlib.h>
#include <imgui.h>
#include <gui/panel.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <algorithm>
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
        m_framebuffer = 0;
        m_windowPosX = 0;
        m_windowPosY = 150;
    }
    
    application::~application() {
        vec2 wSize = m_window->getSize(false);
        vec2 wPos = m_window->getPosition();
        
        if(m_shader) delete m_shader;
        if(m_fs) delete m_fs;
        if(m_window) delete m_window;
        if(m_framebuffer) {
            for(size_t i = 0;i < m_framebuffer->attachments.size();i++) {
                delete m_framebuffer->attachments[i];
            }
            delete m_framebuffer;
        }
        
        string exePath = m_args[0];
        exePath = exePath.substr(0, exePath.find_last_of("/"));
        
        FILE* fp = fopen((exePath + "/config.txt").c_str(), "w");
        if(!fp) {
            printf("WARNING: The application is closing but we can't open the config file to save the modified settings. Sorry...\n");
        } else {
            fprintf(fp, "[PATHS]\n");
            fprintf(fp, "game_data_path '%s'\n", m_dataPath.c_str());
            fprintf(fp, "editor_data_path '%s'\n\n", m_editorDataPath.c_str());
            fprintf(fp, "[WINDOW]\n");
            fprintf(fp, "res_x %d\n", (int)wSize.x);
            fprintf(fp, "res_y %d\n\n", (int)wSize.y);
            fprintf(fp, "pos_x %d\n", (int)wPos.x);
            fprintf(fp, "pos_y %d\n\n", (int)wPos.y);
            fprintf(fp, "[ACTOR VARIABLES]\n");
            for(auto i = m_actor_var_types.begin();i != m_actor_var_types.end();i++) {
                fprintf(fp, "av__%s %s\n", i->first.c_str(), i->second.c_str());
            }
            fclose(fp);
        }
    }
    
    bool application::initialize() {
        if(!load_config()) return false;
        m_window = new window(this, m_windowWidth, m_windowHeight);
        if(!m_window->isOpen()) {
            return false;
        }
        
        glfwSetWindowPos(m_window->getWindow(), m_windowPosX, m_windowPosY);
        
        m_fs = new turokfs(m_dataPath);
        
        m_shader = new shader(this);
        m_shader->attribute("position", 0);
        m_shader->attribute("normal", 1);
        m_shader->attribute("texc", 2);
        m_shader->loadFromFile("shaders/simple.vsh", "shaders/simple.fsh");
        
        vec2 dims = m_window->getSize();
        m_framebuffer = new framebuffer(dims.x * 0.75f, dims.y * 0.7f, true);
        m_framebuffer->attachments.push_back(new texture(dims.x, dims.y, GL_RGB, GL_UNSIGNED_BYTE, true)); //color buffer
        m_framebuffer->attachments.push_back(new texture(dims.x, dims.y, GL_RGB, GL_UNSIGNED_BYTE, true)); //asset_id
        m_framebuffer->attachments.push_back(new texture(dims.x, dims.y, GL_RGB, GL_UNSIGNED_BYTE, true)); //asset_submesh_id
        m_framebuffer->attachments.push_back(new texture(dims.x, dims.y, GL_RGB, GL_UNSIGNED_BYTE, true)); //asset_submesh_chunk_id
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
        else if(name == "pos_x") m_windowPosX = atoi(value.c_str());
        else if(name == "pos_y") m_windowPosY = atoi(value.c_str());
        else if(name.find("av__") != name.npos) {
            string vName = name.substr(4, name.length() - 4);
            define_actor_var_type(vName, value);
        }
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
        else if(e->name == "input_event") {
            input_event* input = (input_event*)e;
        }
        else if(e->name == "window_resize") {
            app_resize_event* evt = (app_resize_event*)e;
        } else if(e->name == "define_av_type") {
            set_actor_var_type_event* evt = (set_actor_var_type_event*)e;
            define_actor_var_type(evt->vname, evt->vtype);
        }
        
        for(auto i = m_panels.begin();i != m_panels.end();i++) {
            (*i)->onEvent(e);
        }
        
        m_window->onEvent(e);
    }
    
    void application::load_level(const string &path) {
        if(m_level) {
            delete m_level;
            dispatchNamedEvent("level_unloaded");
        }
        
        m_level = new level(this);
        
        if(!m_level->load(path)) {
            delete m_level;
            m_level = 0;
        }
        dispatchNamedEvent("level_loaded");
        
        return;
    }
    
    string application::get_actor_var_type(const string &vname) const {
        auto i = m_actor_var_types.find(vname);
        if(i == m_actor_var_types.end()) return "";
        
        return i->second;
    }
    
    int application::run() {
        glEnable(GL_DEPTH_TEST);
        //glEnable(GL_CULL_FACE);
        glDepthFunc(GL_LESS);
        glCullFace(GL_CW);
        while(m_window->isOpen()) {
            m_window->poll();
            m_window->beginFrame();
            
            m_framebuffer->bind();
            m_framebuffer->clear(vec4(0.5f, 0.5f, 0.5f, 1.0f), 1.0f);
            m_shader->bind();
            
            if(m_level) {
                vector<actor*> actors = m_level->actors();
                for(int i = 0;i < actors.size();i++) {
                    actors[i]->render(m_shader);
                }
            }
            
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            //render ImGui windows
            for(auto i = m_panels.begin();i != m_panels.end();i++) {
                (*i)->render();
            }
            
            m_window->endFrame();
        }
        return 0;
    }
}
