#pragma once
#include <Turok4.h>
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
        
        protected:
            application* m_app;
            ATRFile* m_atr;
            vector <actor*> m_actors;
    };
}
