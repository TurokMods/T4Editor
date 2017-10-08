#pragma once
#include <gui/panel.h>
#include <event.h>

#include <imgui.h>
using namespace ImGui;


namespace t4editor {
    class actor_instance_properties : public ui_panel {
        public:
            actor_instance_properties() {
                setName("Actor Instance Properties");
                close();
            }
            ~actor_instance_properties() {
            }
        
            virtual void onAttach(ui_panel* toPanel) {
                setPosition(vec2(50,50));
            }
            virtual void onEvent(event* e) {
                if(e->name == "window_resize") {
                    app_resize_event* resize = (app_resize_event*)e;
                }
                else if(e->name == "prompt_load_level") close();
                else if(e->name == "show_instance_properties") open();
                else if(e->name == "load_level") {
                    close();
                    disconnect();
                }
                
            }
        
            virtual void renderContent() {
                
            }
        
        protected:
    };
}
