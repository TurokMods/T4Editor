#include <app.h>
using namespace t4editor;

#include <imgui.h>
#include <gui/panel.h>

class test_panel : public ui_panel {
    public:
        test_panel() {
        }
        virtual ~test_panel() {
        }
    
    protected:
        virtual void renderContent() {
            ImGui::Text("Testy testola");
        }
};

int main(int argc,const char* argv[]) {
    application app(argc, argv);
    if(app.initialize()) {
        test_panel* panel = new test_panel();
        
        app.getWindow()->add_panel(panel);
        int ret = app.run();
        app.getWindow()->remove_panel(panel);
        
        delete panel;
        return ret;
    }
    return -1;
}
