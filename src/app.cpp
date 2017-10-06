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
        m_fov = 65.0f;
        m_camPos = vec3(0.0f, 0.0f, 0.0f);
        m_camAngles = vec2(0.0f, 0.0f);
        m_proj = perspective(radians(m_fov),1200.0f / 1000.0f,1.0f,10000.0f);
        m_view = eulerAngleXYZ(m_camAngles.y, m_camAngles.x, 0.0f) * translate(m_camPos);
        m_vp = m_proj * m_view;
        m_inputEnabledCounter = 0;
        memset(&m_mouseBtnDown, 0, sizeof(bool) * 3);
        memset(&m_keyDown, 0, sizeof(bool) * 256);
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
        
        double cx, cy;
        glfwGetCursorPos(m_window->getWindow(), &cx, &cy);
        m_curCursor = vec2(cx, cy);
        
        m_fs = new turokfs(m_dataPath);
        
        m_shader = new shader(this);
        m_shader->attribute("position", 0);
        m_shader->attribute("normal", 1);
        m_shader->attribute("texc", 2);
        m_shader->loadFromFile("shaders/simple.vsh", "shaders/simple.fsh");
        
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
        else if(e->name == "disable_input") {
            m_inputEnabledCounter--;
            memset(&m_mouseBtnDown, 0, sizeof(bool) * 3);
            memset(&m_keyDown, 0, sizeof(bool) * 256);
        } else if(e->name == "enable_input") {
            m_inputEnabledCounter++;
            if(m_inputEnabledCounter > 0) m_inputEnabledCounter = 0;
        }
        else if(e->name == "input_event") {
            if(m_inputEnabledCounter < 0) return;
            
            input_event* input = (input_event*)e;
            //printf("input: %d\n", input->type);
            switch(input->type) {
                case input_event::ET_MOUSE_MOVE:
                    if(m_mouseBtnDown[0]) { //left mouse
                        vec2 delta = input->cursorPosition - m_curCursor;
                        float fac = std::max((m_fov - 10.0f) / (170.0f - 10.0f), 0.03f);
                        m_camAngles.x -= delta.x * 0.005f * fac;
                        m_camAngles.y -= delta.y * 0.005f * fac;
                        if(m_camAngles.y < -85.0f) m_camAngles.y = -85.0f;
                        if(m_camAngles.y >  85.0f) m_camAngles.y =  85.0f;
                        if(m_camAngles.x > 360.0f) m_camAngles.x -= 360.0f;
                        if(m_camAngles.x < -360.0f) m_camAngles.x += 360.0f;
                        
                        m_view = eulerAngleXYZ(m_camAngles.y, m_camAngles.x, 0.0f) * translate(m_camPos);
                        m_vp = m_proj * m_view;
                    }
                    m_curCursor = input->cursorPosition;
                    break;
                case input_event::ET_MOUSE_LEFT_DOWN:
                    m_mouseBtnDown[0] = true;
                    break;
                case input_event::ET_MOUSE_LEFT_UP:
                    m_mouseBtnDown[0] = false;
                    break;
                case input_event::ET_MOUSE_MIDDLE_DOWN:
                    m_mouseBtnDown[1] = true;
                    break;
                case input_event::ET_MOUSE_MIDDLE_UP:
                    m_mouseBtnDown[1] = false;
                    break;
                case input_event::ET_MOUSE_RIGHT_DOWN:
                    m_mouseBtnDown[2] = true;
                    break;
                case input_event::ET_MOUSE_RIGHT_UP:
                    m_mouseBtnDown[2] = false;
                    break;
                case input_event::ET_KEY_DOWN:
                    m_keyDown[input->key] = true;
                    break;
                case input_event::ET_KEY_UP:
                    m_keyDown[input->key] = false;
                    break;
                case input_event::ET_SCROLL: {
                    float fac = std::max((m_fov - 10.0f) / (170.0f - 10.0f), 0.01f);
                    m_fov += input->scrollDelta * fac;
                    if(m_fov > 170.0f) m_fov = 170.0f;
                    if(m_fov < 10.0f) m_fov = 10.0f;
                    printf("fov: %f | scroll fac: %f\n", m_fov, fac);
                    m_proj = perspective(radians(m_fov), m_window->getSize().x / m_window->getSize().y, 1.0f, 10000.0f);
                    m_vp = m_proj * m_view;
                    break;
                }
                default:
                    break;
            }
        }
        else if(e->name == "window_resize") {
            app_resize_event* evt = (app_resize_event*)e;
            m_proj = perspective(m_fov, float(evt->new_width) / float(evt->new_height), 1.0f, 1000.0f);
            m_vp = m_proj * m_view;
        }
        
        for(auto i = m_panels.begin();i != m_panels.end();i++) {
            (*i)->onEvent(e);
        }
        
        m_window->onEvent(e);
    }
    
    void application::load_level(const string &path) {
        m_level = new level(this);
        
        if(!m_level->load(path)) {
            delete m_level;
            m_level = 0;
        }
        
        return;
    }
    
    int application::run() {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glCullFace(GL_CW);
        while(m_window->isOpen()) {
            m_window->poll();
            
            float moveSpeed = 1.0f; // units per frame
            vec3 moveDir = vec3(0.0f, 0.0f, 0.0f);
            if(m_keyDown[GLFW_KEY_W]) moveDir.z =  1;
            if(m_keyDown[GLFW_KEY_S]) moveDir.z = -1;
            if(m_keyDown[GLFW_KEY_A]) moveDir.x =  1;
            if(m_keyDown[GLFW_KEY_D]) moveDir.x = -1;
            if((moveDir.x + moveDir.y + moveDir.z) != 0.0f) {
                moveDir = normalize(moveDir) * moveSpeed;
                vec4 nPos = vec4(moveDir, 1.0f) * eulerAngleXYZ(m_camAngles.y, m_camAngles.x, 0.0f);
                m_camPos.x += nPos.x;
                m_camPos.y += nPos.y;
                m_camPos.z += nPos.z;
                
                m_view = eulerAngleXYZ(m_camAngles.y, m_camAngles.x, 0.0f) * translate(m_camPos);
                m_vp = m_proj * m_view;
            }
            
            m_window->beginFrame();
            
            glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            m_shader->bind();
            
            if(m_level) {
                vector<actor*> actors = m_level->actors();
                for(int i = 0;i < actors.size();i++) {
                    actors[i]->position.x = -20.0f + float(i);
                    actors[i]->render(m_shader);
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
