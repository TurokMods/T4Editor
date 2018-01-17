#pragma once
#include <gui/panel.h>
#include <event.h>

#include <imgui.h>
using namespace ImGui;

namespace t4editor {
    class framebuffer_window : public ui_panel {
        public:
            framebuffer_window() {
                setName("Framebuffers");
                setCanResize(false);
                close();
            }
            ~framebuffer_window() {
            }
        
            virtual void onAttach(ui_panel* toPanel) {
                setPosition(vec2(50,50));
                framebuffer* fb = m_app->getFrame();
				vec2 dims = vec2(fb->w, fb->h) * 0.35f;
				setSize(vec2(dims.x, (dims.y * 2.0f) + 25.0f));
            }
            virtual void onEvent(event* e) {
                if(e->name == "window_resize") {
                    app_resize_event* resize = (app_resize_event*)e;
					framebuffer* fb = m_app->getFrame();
					vec2 dims = vec2(fb->w, fb->h) * 0.35f;
					setSize(vec2(dims.x, (dims.y * 2.0f) + 25.0f));
                } else if(e->name == "prompt_framebuffer_view") {
                    open();
				}
            }
        
            virtual void renderContent() {
                framebuffer* fb = m_app->getFrame();
				vec2 dims = vec2(fb->w, fb->h) * 0.35f;
                ImGui::Image((GLuint*)fb->attachments[0]->id, ImVec2(dims.x,dims.y), ImVec2(0, 1), ImVec2(1, 0));
                ImGui::Image((GLuint*)fb->attachments[1]->id, ImVec2(dims.x,dims.y), ImVec2(0, 1), ImVec2(1, 0));
                //ImGui::Image((GLuint*)fb->attachments[2]->id, ImVec2(dims.x,dims.y), ImVec2(0, 1), ImVec2(1, 0));
                //ImGui::Image((GLuint*)fb->attachments[3]->id, ImVec2(dims.x,dims.y), ImVec2(0, 1), ImVec2(1, 0));
            }
    };
}
