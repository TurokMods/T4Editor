#include <app.h>
using namespace t4editor;

#include <imgui.h>
using namespace ImGui;

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
            
            setSize(m_app->getWindow()->getSize());
        }
        virtual void onEvent(event* e) {
            if(e->name == "window_resize") {
                app_resize_event* resize = (app_resize_event*)e;
                setSize(vec2(resize->new_width,resize->new_height));
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

int main(int argc,const char* argv[]) {
    application app(argc, argv);
    if(app.initialize()) {
        main_window app_space;
        app.add_panel(&app_space);
        
        return app.run();
    }
    return -1;
}
