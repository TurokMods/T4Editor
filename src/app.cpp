#include <app.h>
#include <stdio.h>

namespace t4editor {
    application::application(int argc, const char* argv[]) {
        for(int i = 0;i < argc;i++) {
            m_args.push_back(string(argv[i]));
        }
    }
    application::~application() {
    }

    void application::handle_config_var(const string &name, const string &value) {
        if(name == "data_path") m_dataPath = value;
    }

    int application::run() {
        if(!load_config()) return -1;
        printf("Using '%s' as data directory\n", m_dataPath.c_str());
        return 0;
    }
}
