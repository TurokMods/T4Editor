#pragma once
#include <gui/panel.h>
#include <gui/imgui_tabs.h>
#include <event.h>

#include <imgui.h>
using namespace ImGui;

namespace t4editor {
    class bottom_panel : public ui_panel {
        public:
            bottom_panel() {
                setName("LowerPanel");
                setCanClose(false);
                setFlagsManually(ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_ShowBorders);
            }
        
            ~bottom_panel() {
            }
            
        
            virtual void onAttach(ui_panel* toPanel) {
                vec2 sz = m_app->getWindow()->getSize(false);
                sz.x *= LEVEL_VIEW_WIDTH_FRACTION;
                sz.y -= (sz.y * LEVEL_VIEW_HEIGHT_FRACTION);
                
                setPosition(vec2(0, m_app->getWindow()->getSize(false).y - sz.y));
                setSize(sz);
            }
        
            virtual void onEvent(event* e) {
                if(e->name == "window_resize") {
                    vec2 sz = m_app->getWindow()->getSize();
                    sz.x *= LEVEL_VIEW_WIDTH_FRACTION;
                    sz.y -= (sz.y * LEVEL_VIEW_HEIGHT_FRACTION);
                    
                    setPosition(vec2(0, m_app->getWindow()->getSize(false).y - sz.y));
                    setSize(sz);
                }
            }
        
            virtual void renderContent() {
                BeginTabBar("", ImVec2(500, 26));
                    DrawTabsBackground();
                    if(AddTab("Logs")) {
                        Text("logs here");
                    }
                    if(AddTab("other stuff")) {
                        Text("other stuff here");
                    }
                EndTabBar();
            }
    };
}
