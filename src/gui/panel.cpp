#include <gui/panel.h>
#include <imgui.h>
#include <app.h>

 namespace t4editor {
    ui_panel::ui_panel() {
        m_isOpen = true;
        m_wasOpen = false;
        m_bgAlpha = 1.0f;
        m_canResize = true;
        m_canMove = true;
        m_sizeUpdated = false;
        m_hasTitleBar = true;
        m_posUpdated = false;
        m_canCollapse = false;
        m_isWindow = true;
        m_canClose = true;
        m_parent = nullptr;
        m_app = nullptr;
        m_manualFlags = 0;
    }
    ui_panel::~ui_panel() {
    }
    
    void ui_panel::add_panel(ui_panel *panel) {
        m_panels.push_back(panel);
        panel->m_parent = this;
        panel->m_app = m_app;
        panel->onAttach(this);
    }
    
    void ui_panel::remove_panel(ui_panel *panel) {
        for(auto i = m_panels.begin();i != m_panels.end();i++) {
            if((*i) == panel) {
                m_panels.erase(i);
                panel->m_parent = nullptr;
                return;
            }
        }
    }
    
    void ui_panel::render() {
        if(m_isOpen) {
            if(!m_wasOpen) {
                this->dispatchNamedEvent("opened");
                m_wasOpen = true;
            }
            ImGuiWindowFlags f = 0;
            if(!m_canResize) f |= ImGuiWindowFlags_NoResize;
            if(!m_canMove) f |= ImGuiWindowFlags_NoMove;
            if(!m_hasTitleBar) f |= ImGuiWindowFlags_NoTitleBar;
            if(!m_canCollapse) f |= ImGuiWindowFlags_NoCollapse;
            
            if(m_manualFlags != 0) f = m_manualFlags;
        
            if(m_isWindow) {
                if(!m_parent) ImGui::Begin(m_name.c_str(), m_canClose ? &m_isOpen : nullptr, ImVec2(0.0f, 0.0f), m_bgAlpha, f);
                else ImGui::BeginChild(m_name.c_str(), ImVec2(m_initSize.x, m_initSize.y), f);
            
                
                if(m_sizeUpdated) {
                    ImGui::SetWindowSize(ImVec2(m_initSize.x, m_initSize.y));
                    m_sizeUpdated = false;
                }
                
                if(m_posUpdated) {
                    ImGui::SetWindowPos(ImVec2(m_initPos.x, m_initPos.y));
                    m_posUpdated = false;
                }
                
                ImVec2 sz = ImGui::GetWindowSize();
                m_curSize = vec2(sz.x, sz.y);
                
                ImVec2 pos = ImGui::GetWindowPos();
                m_curPos = vec2(pos.x, pos.y);
                
                // render children
                for(auto i = m_panels.begin();i != m_panels.end();i++) {
                    (*i)->render();
                }
                
                renderContent();
            
                if(!m_parent) ImGui::End();
                else ImGui::EndChild();
            }
            else {
                // render children
                for(auto i = m_panels.begin();i != m_panels.end();i++) {
                    (*i)->render();
                }
                
                renderContent();
            }
        } else {
            if(m_wasOpen) {
                this->dispatchNamedEvent("closed");
                m_wasOpen = false;
            }
        }
    }
 }
