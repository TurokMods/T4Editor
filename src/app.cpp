#include <app.h>
#include <stdio.h>
#include <stdlib.h>
#include <imgui.h>
#include <gui/panel.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <algorithm>
#include <cassert>
#include <logger.h>

#include <render/SOIL/SOIL.h>
#include <util/json.hpp>
#include <iostream>
#include <fstream>


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
        m_defaultTex = 0;
        m_updatingCache = false;
        m_numDrawCalls = 0;
        m_Camera = new Camera(90, m_windowWidth / m_windowHeight, 1.0f, 1000.0f);
    }
    
    application::~application() {
        vec2 wSize = m_window->getSize(false);
        vec2 wPos = m_window->getPosition();
        
        if(m_shader) delete m_shader;
        if(m_fs) delete m_fs;
        if (m_window) delete m_window;
        if (m_framebuffer) {
            for (size_t i = 0; i < m_framebuffer->attachments.size(); i++) {
                delete m_framebuffer->attachments[i];
            }
            delete m_framebuffer;
        }

        nlohmann::json j;
        j["editor_data_path"] = m_editorDataPath;
        j["game_data_path"] = m_dataPath;
        j["res_x"] = (int)wSize.x;
        j["res_y"] = (int)wSize.y;
        j["pos_x"] = (int)wPos.x;
        j["pos_y"] = (int)wPos.y;
        for (auto i = m_actor_var_types.begin(); i != m_actor_var_types.end(); i++) {
            string key = "av__" + i->first;
            j[key.c_str()] = i->second.c_str();
        }

        for (auto i = m_actor_block_types.begin(); i != m_actor_block_types.end(); i++) {
            string key = "ab__" + i->first;
            j[key.c_str()] = i->second.c_str();
        }

        string path = string(m_args[0]);
        size_t lidx = path.find_last_of('\\');
        if(lidx == path.npos) lidx = path.find_last_of('/');
        path = path.substr(0, lidx);

        printf("Writing json data: %s\n", j.dump().c_str());
        std::ofstream o(path + '/' + "config.json");
        o << std::setw(4) << j << std::endl;
        o.close();

        if (m_level) delete m_level;
    }

    bool application::initialize() {
        if (!load_config()) return false;
        m_window = new window(this, m_windowWidth, m_windowHeight);
        if (!m_window->isOpen()) {
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

        u32* defaultTexData = new u32[2 * 2];
        for (u8 x = 0; x < 2; x++) {
            for (u8 y = 0; y < 2; y++) {
                u8* pixel = (u8*)(&defaultTexData[x + (y * 2)]);
                if ((x + (y * 2)) % 2 == 0) {
                    pixel[0] = 100;
                    pixel[1] = 20;
                    pixel[2] = 200;
                    pixel[3] = 255;
                }
                else {
                    pixel[0] = 210;
                    pixel[1] = 210;
                    pixel[2] = 210;
                    pixel[3] = 210;
                }
            }
        }
        m_defaultTex = new texture(2, 2, GL_RGBA, GL_UNSIGNED_BYTE, false, (u8*)defaultTexData);
        delete[] defaultTexData;
        m_defaultTex->bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);

        return true;
    }

    bool application::load_config() {
        nlohmann::json j;
        std::ifstream i("config.json");
        i >> j;

        for (nlohmann::json::iterator it = j.begin(); it != j.end(); it++) {
            std::string name = it.key();
            if (name == "game_data_path") {
                m_dataPath = it.value().get<string>();
                SetTurokDirectory(m_dataPath);
                printf("Using '%s' as game data directory\n", m_dataPath.c_str());
            }
            else if (name == "editor_data_path") {
                m_editorDataPath = it.value().get<string>();
                printf("Using '%s' as editor data directory\n", m_editorDataPath.c_str());
            }
            else if (name == "res_x") m_windowWidth = it.value().get<int>();
            else if (name == "res_y") m_windowHeight = it.value().get<int>();
            else if (name == "pos_x") m_windowPosX = it.value().get<int>();
            else if (name == "pos_y") m_windowPosY = it.value().get<int>();
            else if (name.find("av__") != name.npos) {
                string vName = name.substr(4, name.length() - 4);
                define_actor_var_type(vName, it.value());
            }
            else if (name.find("ab__") != name.npos) {
                string bName = name.substr(4, name.length() - 4);
                define_actor_block_type(bName, it.value());
            }
        }

        i.close();
        return true;
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
            //input_event* input = (input_event*)e;
        }
        else if(e->name == "window_resize") {
            //app_resize_event* evt = (app_resize_event*)e;
        } else if(e->name == "define_av_type") {
            set_actor_var_type_event* evt = (set_actor_var_type_event*)e;
            define_actor_var_type(evt->vname, evt->vtype);
        } else if(e->name == "define_ab_type") {
            set_actor_block_type_event* evt = (set_actor_block_type_event*)e;
            define_actor_block_type(evt->bname, evt->btype);
        } else if(e->name == "actor_added") {
            m_level->actor_added();
        } else if(e->name == "save_level") {
            if(m_level) m_level->levelFile()->Save(m_dataPath + "/" + m_level->levelFile()->GetFileName());
            else printf("No level loaded\n");
        } else if(e->name == "update_actor_cache") {
            update_actor_cache();
            m_updatingCache = true;
            m_cacheProgress = 0.0f;
        } else if(e->name == "actor_selection") {
            //trigger_repaint();
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
        //m_viewChanged = true;
        return;
    }
    
    string application::get_actor_var_type(const string &vname) const {
        auto i = m_actor_var_types.find(vname);
        if(i == m_actor_var_types.end()) return "";
        
        return i->second;
    }
    string application::get_actor_block_type(const string &vname) const {
        auto i = m_actor_block_types.find(vname);
        if(i == m_actor_block_types.end()) return "";
        
        return i->second;
    }

    texture* application::getTexture(std::string filename) {
        auto found = m_textures.find(filename);
        if (found == m_textures.end()) {
            texture* t = this->loadTexture(filename);
            m_textures.insert(std::make_pair(filename, t));
            printf("Loading new texture %s\n", filename.c_str());
            return t;
        }
        else {
            printf("Texture already found, returning %s\n", filename.c_str());
            return found->second;
        }
    }

    texture* application::loadTexture(std::string filename) {
        i32 w, h, ch;
        unsigned char* Data = SOIL_load_image(filename.c_str(), &w, &h, &ch, 4);
        if (Data)
        {
            texture* t = new texture(w, h, GL_RGBA, GL_UNSIGNED_BYTE, false, Data);
            SOIL_free_image_data(Data);
            return t;
        }
        return 0;
    }

    int application::run() {
        glEnable(GL_DEPTH_TEST);
        //glEnable(GL_CULL_FACE);
        glDepthFunc(GL_LESS);
        glCullFace(GL_CW);
        while(m_window->isOpen()) {
            if(m_updatingCache) {
                m_cacheProgress = m_fs->get_cache_progress(m_updatingCache, &m_cacheLastFile);
                m_updatingCache = !m_updatingCache;
                if(!m_updatingCache) {
                    m_cacheLastFile = "";
                    dispatchNamedEvent("actor_caching_completed");
                }
            }

            m_window->poll();
            m_window->beginFrame();
            
            //if(m_viewChanged)
            {
                m_framebuffer->bind();
                m_framebuffer->clear(vec4(0.5f, 0.5f, 0.5f, 1.0f), 1.0f);
                m_shader->bind();
            
                Frustum f = m_Camera->GetFrustum();

                if(m_level) {
                    vector<actor*> actors = m_level->actors();
                    if (actors.size() > 0) {
                        actors[0]->render(m_shader, m_Camera->GetView(), m_Camera->GetViewProj());
                        for (int i = 1; i < actors.size(); i++) {
                            if (f.intersectFrustumAABB(actors[i]->getBoundingBox()))
                            {
                                actors[i]->render(m_shader, m_Camera->GetView(), m_Camera->GetViewProj());
                            }
                        }
                    }
                }

                //m_viewChanged = false;
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
