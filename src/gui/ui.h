#pragma once
#include <imgui.h>
#include <imgui_internal.h>
using namespace ImGui;

#include <app.h>
#include <gui/panel.h>
#include <event.h>

#define LEVEL_VIEW_WIDTH_FRACTION 0.8f
#define LEVEL_VIEW_HEIGHT_FRACTION 0.7f

namespace t4editor {
    class application;
    void register_ui(application* app);
    void destroy_ui();
}
