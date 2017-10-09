#pragma once
#include <Turok4.h>
#include <render/texture.h>
using namespace opent4;

#include <string>
#include <vector>
using namespace std;

namespace t4editor {
    class actor;
    class application;
    class level {
        public:
            level(application* app);
            ~level();
        
            bool load(const string& file);
        
            vector<actor*> actors() const { return m_actors; }
        
            ATRFile* levelFile() const { return m_atr; }

			texture* loadTexture(std::string file_path);
        
        protected:
            application* m_app;
            ATRFile* m_atr;
            vector <actor*> m_actors;
    };
}
