#include <app.h>
#include <stdio.h>

namespace t4editor {
    application::application(int argc, const char* argv[]) {
        for(int i = 0;i < argc;i++) {
            m_args.push_back(string(argv[i]));
        }
        m_windowWidth = 1200;
        m_windowHeight = 900;
        m_window = 0;
    }
    application::~application() {
        if(m_window) delete m_window;
    }

    void application::handle_config_var(const string &name, const string &value) {
        if(name == "data_path") m_dataPath = value;
        else if(name == "res_x") m_windowWidth = atoi(value.c_str());
        else if(name == "res_y") m_windowHeight = atoi(value.c_str());
    }

    int application::run() {
        if(!load_config()) return -1;
        m_window = new window(m_windowWidth, m_windowHeight);
        if(!m_window->isOpen()) {
            delete m_window;
            return -1;
        }

        printf("Using '%s' as data directory\n", m_dataPath.c_str());
        return 0;
    }
}
