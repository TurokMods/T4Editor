#include <gui/panel.h>
#include <imgui.h>

 namespace t4editor {
    ui_panel::ui_panel() {
    }
    ui_panel::~ui_panel() {
    }
    
    void ui_panel::render() {
        ImGui::Begin(m_name.c_str());
        renderContent();
        ImGui::End();
    }
 }
