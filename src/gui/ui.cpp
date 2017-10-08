#include <app.h>

#include <gui/ui.h>
#include <gui/main_window.h>
#include <gui/load_level.h>
#include <gui/framebuffer_view.h>
#include <gui/actor_instance_properties.h>
#include <gui/level_view.h>
#include <gui/bottom_panel.h>
#include <gui/sidebar.h>

namespace t4editor {
    main_window* app_space;
    level_window* load_level;
    framebuffer_window* framebuffer_view;
    actor_instance_properties* actor_instance_info;
    level_view* level_display;
    bottom_panel* lower_panel;
    sidebar* tools;
    
    void register_ui(application* app) {
        app_space = new main_window();
        app->add_panel(app_space);
        
        load_level = new level_window();
        app->add_panel(load_level);
        
        framebuffer_view = new framebuffer_window();
        app->add_panel(framebuffer_view);
        
        actor_instance_info = new actor_instance_properties();
        app->add_panel(actor_instance_info);
        
        level_display = new level_view();
        app->add_panel(level_display);
        
        lower_panel = new bottom_panel();
        app->add_panel(lower_panel);
        
        tools = new sidebar();
        app->add_panel(tools);
    }
    
    void destroy_ui() {
        delete tools;
        delete lower_panel;
        delete level_display;
        delete actor_instance_info;
        delete framebuffer_view;
        delete load_level;
        delete app_space;
    }
}
