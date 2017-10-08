#pragma once
#include <gui/panel.h>
#include <event.h>

#include <imgui.h>
using namespace ImGui;

#include <Turok4.h>
using namespace opent4;

namespace t4editor {
    class actor_instance_properties : public ui_panel {
        public:
            actor_instance_properties() {
                setName("Actor Instance Properties");
                close();
            }
            ~actor_instance_properties() {
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
                setPosition(vec2(50,50));
            }
            virtual void onEvent(event* e) {
                if(e->name == "window_resize") {
                    app_resize_event* resize = (app_resize_event*)e;
                }
                else if(e->name == "prompt_load_level") close();
                else if(e->name == "show_instance_properties") open();
                else if(e->name == "load_level") {
                    close();
                    disconnect();
                }
                if(e->name == "actor_selection") {
                    actor_selection_event* evt = (actor_selection_event*)e;
                    if(evt->deselected) {
                        close();
                        disconnect();
                    }
                    else {
                        connect(evt);
                        open();
                    }
                }
            }
        
            virtual void renderContent() {
                if(Actor) {
                    if(Actor->actorTraits) {
                        //actor instance
                        Text("%s", Actor->actorTraits->Name.c_str());
                        
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
        
        protected:
            level* Level;
            actor* Actor;
            actor_mesh* actorMesh;
            int actorId;
            int subMeshId;
            int meshChunkId;
    };
}
