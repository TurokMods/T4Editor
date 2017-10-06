#pragma once
#include <gui/panel.h>
#include <event.h>

#include <imgui.h>
using namespace ImGui;

namespace t4editor {
    class main_window : public ui_panel {
        public:
            main_window() {
            }
            ~main_window() {
            }
        
            virtual void onAttach(ui_panel* toPanel) {
                setPosition(vec2(0,0));
                setHasTitleBar(false);
                setCanClose(false);
                setCanCollapse(false);
                setFlagsManually(ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar);
                
                setSize(vec2(m_app->getWindow()->getSize().x,0.0f));
            }
            virtual void onEvent(event* e) {
                if(e->name == "window_resize") {
                    app_resize_event* resize = (app_resize_event*)e;
                    setSize(vec2(resize->new_width,0.0f));
                }
            }
        
            virtual void renderContent() {
                if(BeginMenuBar()) {
                    if(BeginMenu("File")) {
                        if(MenuItem("Load Level")) {
                            m_app->dispatchNamedEvent("prompt_load_level");
                        }
                        if(MenuItem("Exit")) {
                            m_app->dispatchNamedEvent("shutdown");
                        }
                        EndMenu();
                    }
                    if(BeginMenu("Help")) {
                        if(MenuItem("Delete system 32")) {
                            m_app->dispatchNamedEvent("shutdown");
                        }
                        EndMenu();
                    }
                    EndMenuBar();
                }
            }
    };
}
