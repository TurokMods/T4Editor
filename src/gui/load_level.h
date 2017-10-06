#pragma once
#include <gui/panel.h>
#include <event.h>

#include <imgui.h>
using namespace ImGui;

namespace t4editor {
    class level_window : public ui_panel {
        public:
            level_window() {
                setName("Select Level Actor");
                setPosition(vec2(50, 50));
                setCanResize(false);
                setSize(vec2(820, 300));
                close();
            }
            ~level_window() {
            }
        
            virtual void onAttach(ui_panel* toPanel) {
            }
            virtual void onEvent(event* e) {
                if(e->name == "prompt_load_level") open();
                if(e->name == "opened") m_app->dispatchNamedEvent("disable_input");
                if(e->name == "closed") m_app->dispatchNamedEvent("enable_input");
            }
        
            virtual void renderContent() {
                const turokfs* fs = m_app->getTurokData();
                vector<level_entry*> levels = fs->levels();
                
                BeginChild("level_view");
                    for(auto i = levels.begin();i != levels.end();i++) {
                        string name = (*i)->name();
                        string path = (*i)->path();
                        if(Selectable(name.c_str())) {
                            load_level_event e(path);
                            m_app->onEvent(&e);
                            close();
                        }
                        SameLine(200);
                        Text("%s", path.c_str());
                    }
                EndChild();
            }
    };
}
