#include <app.h>
#include <stdio.h>
#include <stdlib.h>
#include <imgui.h>

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
        m_window = new window(m_windowWidth, m_windowHeight);
        if(!m_window->isOpen()) {
            return false;
        }
        
        m_fs = new turokfs(m_dataPath);
        return true;
    }
    void application::handle_config_var(const string &name, const string &value) {
        if(name == "data_path") {
            m_dataPath = value;
            printf("Using '%s' as data directory\n", m_dataPath.c_str());
        }
        else if(name == "res_x") m_windowWidth = atoi(value.c_str());
        else if(name == "res_y") m_windowHeight = atoi(value.c_str());
    }

    int application::run() {
        while(m_window->isOpen()) {
            m_window->poll();
            
            m_window->beginFrame();
            glClearColor(0, 0, 0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT);
            
            m_window->endFrame();
        }
        return 0;
    }
}
