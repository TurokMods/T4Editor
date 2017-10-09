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
                setCanClose(false);
                setFlagsManually(ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_NoCollapse);
                
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
                sz.x -= (sz.x * LEVEL_VIEW_WIDTH_FRACTION);
                
                setPosition(vec2(m_app->getWindow()->getSize().x * LEVEL_VIEW_WIDTH_FRACTION, 20.0f));
                setSize(sz);
            }
        
            virtual void onEvent(event* e) {
                if(e->name == "window_resize") {
                    vec2 sz = m_app->getWindow()->getSize();
                    sz.x -= (sz.x * LEVEL_VIEW_WIDTH_FRACTION);
                    
                    setPosition(vec2(m_app->getWindow()->getSize().x * LEVEL_VIEW_WIDTH_FRACTION, 20.0f));
                    setSize(sz);
                } else if(e->name == "actor_selection") {
                    actor_selection_event* evt = (actor_selection_event*)e;
                    if(evt->deselected) disconnect();
                    else connect(evt);
                }
            }
        
            virtual void renderContent() {
                if(Actor) {
                    if(Actor->actorTraits) {
                        Text("Actor: %s", Actor->actorTraits->Name.c_str());
                        
                        if(CollapsingHeader("Transform")) {
                            Columns(2);
                                Indent(10.0f);
                                    PushItemWidth(GetColumnWidth() - 16.0f);
                                        DragFloat3("##pos", &Actor->actorTraits->Position.x);
                                        DragFloat3("##rot", &Actor->actorTraits->Rotation.x);
                                        DragFloat3("##sca", &Actor->actorTraits->Scale.x);
                                    PopItemWidth();
                                Unindent(10.0f);
                            NextColumn();
                                ImVec2 cp = GetCursorPos();
                                SetCursorPos(ImVec2(cp.x, cp.y + 2.0f));
                                Text("Position");
                                cp = GetCursorPos();
                                SetCursorPos(ImVec2(cp.x, cp.y + 2.0f));
                                cp = GetCursorPos();
                                SetCursorPos(ImVec2(cp.x, cp.y + 2.0f));
                                Text("Rotation");
                                cp = GetCursorPos();
                                SetCursorPos(ImVec2(cp.x, cp.y + 2.0f));
                                cp = GetCursorPos();
                                SetCursorPos(ImVec2(cp.x, cp.y + 2.0f));
                                Text("Scale");
                                cp = GetCursorPos();
                                SetCursorPos(ImVec2(cp.x, cp.y + 2.0f));
                            EndColumns();
                        }
                    } else {
                        string file = TransformPseudoPathToRealPath(Level->levelFile()->GetActorMeshFile());
                        int last_slash = file.find_last_of("/");
                        file = file.substr(last_slash + 1, file.length() - (last_slash + 1));
                        Text("Level: %s", file.c_str());
                    }
                }
                else Text("No selection");
                
                renderLocalActorVariables();
                renderGlobalActorVariables();
                renderActorData();
            }
        
            void renderLocalActorVariables(); //Stored in the ATI file
            void renderGlobalActorVariables(); //Stored in the actor's ATR file
            void renderActorData();
        
        protected:
            level* Level;
            actor* Actor;
            actor_mesh* actorMesh;
            int actorId;
            int subMeshId;
            int meshChunkId;
    };
}
