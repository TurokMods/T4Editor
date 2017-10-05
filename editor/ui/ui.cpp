#include <app.h>

#include <ui/ui.h>
#include <ui/main_window.h>
#include <ui/load_level.h>

namespace t4editor {
    main_window* app_space;
    level_window* load_level;
    
    void register_ui(application* app) {
        app_space = new main_window();
        app->add_panel(app_space);
        
        load_level = new level_window();
        app->add_panel(load_level);
    }
    
    void destroy_ui() {
        delete load_level;
        delete app_space;
    }
}
