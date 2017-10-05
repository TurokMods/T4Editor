#pragma once
#include <Turok4.h>
using namespace opent4;

#include <string>
#include <vector>
using namespace std;

namespace t4editor {
    class actor;
    class level {
        public:
            level();
            ~level();
        
            bool load(const string& file);
        
            vector<actor*> actors() const { return m_actors; }
        
        protected:
            ATRFile* m_atr;
            vector <actor*> m_actors;
    };
}
