#include <app.h>

#include <gui/ui.h>
#include <gui/main_window.h>
#include <gui/load_level.h>
#include <gui/framebuffer_view.h>

namespace t4editor {
    main_window* app_space;
    level_window* load_level;
    framebuffer_window* framebuffer_view;
    
    void register_ui(application* app) {
        app_space = new main_window();
        app->add_panel(app_space);
        
        load_level = new level_window();
        app->add_panel(load_level);
        
        framebuffer_view = new framebuffer_window();
        app->add_panel(framebuffer_view);
    }
    
    void destroy_ui() {
        delete load_level;
        delete app_space;
    }
}
