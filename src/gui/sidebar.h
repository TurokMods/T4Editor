#pragma once
#include <gui/panel.h>
#include <gui/imgui_tabs.h>
#include <event.h>

#include <imgui.h>
using namespace ImGui;

#include <Turok4.h>
using namespace opent4;

namespace t4editor {
    class sidebar : public ui_panel {
        public:
            sidebar() {
                setName("Tools View");
                setCanResize(false);
                setCanMove(false);
                setCanClose(false);
                
                Level = 0;
                Actor = 0;
                actorId = 0;
                subMeshId = 0;
                meshChunkId = 0;
            }
        
            ~sidebar() {
            }
        
            void disconnect() {
                Level = 0;
                Actor = 0;
                actorId = 0;
                subMeshId = 0;
                meshChunkId = 0;
            }
        
            void connect(actor_selection_event* evt) {
                Level = evt->parentLevel;
                Actor = evt->selectedActor;
                actorId = evt->actorId;
                subMeshId = evt->subMeshId;
                meshChunkId = evt->meshChunkId;
            }
        
            virtual void onAttach(ui_panel* toPanel) {
                vec2 sz = m_app->getWindow()->getSize();
                sz.y *= 0.7f;
                sz.y -= 2;
                sz.x -= (sz.x * 0.75f);
                
                setPosition(vec2(m_app->getWindow()->getSize().x * 0.75f, 20.0f));
                setSize(sz);
            }
        
            virtual void onEvent(event* e) {
                if(e->name == "window_resize") {
                    vec2 sz = m_app->getWindow()->getSize();
                    sz.y *= 0.7f;
                    sz.x -= (sz.x * 0.75f);
                    
                    setPosition(vec2(m_app->getWindow()->getSize().x * 0.75f, 20.0f));
                    setSize(sz);
                } else if(e->name == "actor_selection") {
                    actor_selection_event* evt = (actor_selection_event*)e;
                    if(evt->deselected) disconnect();
                    else connect(evt);
                }
            }
        
            virtual void renderContent() {
                if(Actor) Text("Actor: %s", Actor->actorTraits->Name.c_str());
                else Text("No selection");
                renderActorVariables();
            }
        
            void renderActorVariables() {
                if(CollapsingHeader("Actor Variables", ImGuiTreeNodeFlags_Framed)) {
                    if(Actor) {
                        if(Actor->actorTraits) {
                            //actor instance
                            ActorVariables* vars = Actor->actorTraits->variables();
                            
                            BeginChild("av_view");
                            if(vars) {
                                for(size_t i = 0;i < vars->GetBlockCount();i++) {
                                    Block* var = vars->GetBlock(i);
                                    
                                    string name = var->GetTypeString();
                                    string data = var->GetData()->GetString();
                                    Selectable(name.c_str());
                                    SameLine(200);
                                    Text("%s", data.c_str());
                                }
                            } else {
                                Selectable("No Variables Set");
                            }
                            EndChild();
                        } else {
                            //level
                            Text("No data to show yet");
                        }
                    } else {
                        Text("No actor selected");
                    }
                }
            }
        
        protected:
            level* Level;
            actor* Actor;
            actor_mesh* actorMesh;
            int actorId;
            int subMeshId;
            int meshChunkId;
    };
}
