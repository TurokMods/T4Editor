#include <app.h>
using namespace t4editor;

#include <gui/ui.h>
#include <util/json.hpp>

int main(int argc,const char* argv[]) {
    application app(argc, argv);
    if(app.initialize()) {
        register_ui(&app);
        int r = app.run();
        destroy_ui();
        return r;
    }
    return -1;
}
