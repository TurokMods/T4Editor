#include <app.h>
using namespace t4editor;

#include <imgui.h>
#include <gui/panel.h>

class test_panel : public ui_panel {
    public:
        test_panel(bool doShowDemo) : m_showDemo(doShowDemo) {
        }
        virtual ~test_panel() {
        }
    
        virtual void onAttach(ui_panel* toPanel) {
            printf("Panel attached to something\n");
        }
        virtual void onEvent(event* e) {
            printf("Event: %s\n", e->name.c_str());
        }
    
    protected:
        virtual void renderContent() {
            ImGui::Text("Testy testola");
            if(m_showDemo) ImGui::ShowTestWindow();
        }
        bool m_showDemo;
};

int main(int argc,const char* argv[]) {
    application app(argc, argv);
    if(app.initialize()) {
        test_panel* panel = new test_panel(true);
        panel->setName("Test Panel");
        panel->setCanResize(false);
        panel->setCanMove(false);
        panel->setCanClose(false);
        panel->setOpacity(0.5f);
        panel->setSize(vec2(290, 190));
        panel->setPosition(vec2(5.0f, 5.0f));
        
        test_panel* inner_panel = new test_panel(false);
        inner_panel->setHasTitleBar(false);
        inner_panel->setCanResize(false);
        inner_panel->setSize(vec2(280, 24));
        inner_panel->setPosition(vec2(0.0f, 0.0f));
        panel->add_panel(inner_panel);
        
        app.add_panel(panel);
        
        int ret = app.run();
        
        app.remove_panel(panel);
        panel->remove_panel(inner_panel);
        
        delete inner_panel;
        delete panel;
        return ret;
    }
    return -1;
}
