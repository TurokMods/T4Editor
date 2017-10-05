#pragma once
#include <string>
using namespace std;

namespace t4editor {
    class ui_panel {
        public:
            ui_panel();
            virtual ~ui_panel();
        
            void setName(const string& name) { m_name = name; }
            string name() const { return m_name; }
        
            void render();
        
        protected:
            virtual void renderContent() = 0;
            string m_name;
    };
}
