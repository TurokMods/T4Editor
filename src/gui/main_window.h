#pragma once
#include <gui/panel.h>
#include <event.h>

#include <imgui.h>
using namespace ImGui;

namespace t4editor {
    class main_window : public ui_panel {
        public:
            main_window() {
                setIsWindow(false);
            }
            ~main_window() {
            }
        
            virtual void onAttach(ui_panel* toPanel) {
                setPosition(vec2(0,0));
                setHasTitleBar(false);
                setCanClose(false);
                setCanCollapse(false);
                setFlagsManually(ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar);
                
                setSize(vec2(m_app->getWindow()->getSize(false).x,0.0f));
            }
            virtual void onEvent(event* e) {
                if(e->name == "window_resize") {
                    app_resize_event* resize = (app_resize_event*)e;
                    setSize(vec2(resize->new_width,0.0f));
                }
            }
        
            virtual void renderContent() {
                if(BeginMainMenuBar()) {
                    if(BeginMenu("File")) {
                        if(MenuItem("Load Level")) {
                            m_app->dispatchNamedEvent("prompt_load_level");
                        }
						if(MenuItem("Save Level")) {
							m_app->dispatchNamedEvent("save_level");
						}
						if(MenuItem("Update Actor Cache")) {
							m_app->dispatchNamedEvent("update_actor_cache");
						}
                        if(MenuItem("Exit")) {
                            m_app->dispatchNamedEvent("shutdown");
                        }
                        EndMenu();
                    }
                    if(BeginMenu("View")) {
                        //if(BeginMenu("Windows")) {
                            //I'll just let this be open by default...
                            //if(MenuItem("Level View")) {
                            //    m_app->dispatchNamedEvent("show_level_view");
                            //}
                            //This one too...
                            //if(MenuItem("Actor Instance Properties")) {
                            //    m_app->dispatchNamedEvent("show_instance_properties");
                            //}
                        //    EndMenu();
                        //}
                        if(BeginMenu("Debug")) {
                            if(MenuItem("Framebuffer")) {
                                m_app->dispatchNamedEvent("prompt_framebuffer_view");
                            }
                            EndMenu();
                        }
                        EndMenu();
                    }
                    if(BeginMenu("Help")) {
                        if(MenuItem("Delete system 32")) {
                            m_app->dispatchNamedEvent("shutdown");
                        }
                        EndMenu();
                    }
                    
                    Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

					if(m_app->is_updating_cache()) {
						char buf[256];
						memset(buf,0,256);
						ImVec2 cp = GetCursorPos();
						ProgressBar(m_app->get_cache_update_progress(), ImVec2(-1.0f, 0), "");
						SetCursorPos(ImVec2(cp.x + 5, cp.y));
						Text("(%d\%%) %s",(int)(m_app->get_cache_update_progress() * 100.0f), m_app->get_last_file_cached().c_str());
					}
                    
                    EndMainMenuBar();
                }
            }
    };
}
