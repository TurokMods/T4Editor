#pragma once

#define LEVEL_VIEW_WIDTH_FRACTION 0.8f
#define LEVEL_VIEW_HEIGHT_FRACTION 0.7f

namespace t4editor {
    class application;
    void register_ui(application* app);
    void destroy_ui();
}
