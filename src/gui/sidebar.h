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
                sz.y *= LEVEL_VIEW_HEIGHT_FRACTION;
                sz.y -= 3;
                sz.x -= (sz.x * LEVEL_VIEW_WIDTH_FRACTION);
                
                setPosition(vec2(m_app->getWindow()->getSize().x * LEVEL_VIEW_WIDTH_FRACTION, 20.0f));
                setSize(sz);
            }
        
            virtual void onEvent(event* e) {
                if(e->name == "window_resize") {
                    vec2 sz = m_app->getWindow()->getSize();
                    sz.y *= LEVEL_VIEW_HEIGHT_FRACTION;
                    sz.y -= 3;
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
                    if(Actor->actorTraits) Text("Actor: %s", Actor->actorTraits->Name.c_str());
                    else Text("%s", Level->levelFile()->GetActorMeshFile().c_str());;
                }
                else Text("No selection");
                renderActorVariables();
            }
        
            void renderActorVariables() {
                int count = 0;
                int knownCount = 0;
                int unknownCount = 0;
                if(Actor && Actor->actorTraits && Actor->actorTraits->variables()) {
                    ActorVariables* vars = Actor->actorTraits->variables();
                    count = vars->GetBlockCount();
                    for(size_t i = 0;i < vars->GetBlockCount();i++) {
                        Block* var = vars->GetBlock(i);

                        string name = var->GetTypeString();
                        string set_type = m_app->get_actor_var_type(name);
                        if(set_type == "") unknownCount++;
                        else knownCount++;
                    }
                }
                
                bool varsOpen = CollapsingHeader("Actor Variables", ImGuiTreeNodeFlags_Framed);
                SameLine(125);
                ImVec2 p = GetCursorPos();
                SetCursorPos(ImVec2(p.x, p.y + 2));
                Text("(%d)", count);
                if(varsOpen) {
                    BeginChild("av_sec");
                        ImVec2 dpos = GetWindowPos();
                        SetWindowPos(ImVec2(dpos.x + 10,dpos.y));
                        if(Actor) {
                            if(Actor->actorTraits) {
                                //actor instance
                                ActorVariables* vars = Actor->actorTraits->variables();
                                
                                if(vars) {
                                    bool unkOpen = CollapsingHeader("Unknown Types", ImGuiTreeNodeFlags_Framed);
                                    SameLine(126);
                                    p = GetCursorPos();
                                    SetCursorPos(ImVec2(p.x, p.y + 2));
                                    Text("(%d)", unknownCount);
                                    if(unkOpen) {
                                        BeginChild("av_sec_unk", ImVec2((getSize().x - 15), 400), true);
                                            ImVec2 ukpos = GetWindowPos();
                                            SetWindowPos(ImVec2(ukpos.x + 10,ukpos.y));
                                            for(size_t i = 0;i < vars->GetBlockCount();i++) {
                                                Block* var = vars->GetBlock(i);
                                                
                                                string name = var->GetTypeString();
                                                string set_type = m_app->get_actor_var_type(name);
                                                
                                                ByteStream* data = var->GetData();
                                                void* ptr = data->Ptr();
                                                size_t sz = data->GetSize();
                                                const char* types[13] = { "string", "bool", "int", "float", "vec2", "ivec2", "vec3", "ivec3", "rgb", "vec4", "ivec4", "rgba" };
                                                if(set_type == "") {
                                                    //AV has unknown type, present options
                                                    if(CollapsingHeader(name.c_str())) {
                                                        float ht = 88.0f;
                                                        if(sz == 1) ht = 22.0f;
                                                        else if(sz == 4) ht = 66.0f;
                                                        else if(sz == 8) ht = 66.0f;
                                                        else if(sz == 12) ht = 88.0f;
                                                        else if(sz == 16)  ht = 88.0f;
                                                        else ht = 22.0f;
                                                    
                                                        BeginChild((name + "unk_inputs").c_str(), ImVec2(getSize().x - 35, ht), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
                                                            ImVec2 spos = GetWindowPos();
                                                            SetWindowPos(ImVec2(spos.x + 1, spos.y));
                                                            if(sz <= 64) {
                                                                vector<int> validTypeIndices;
                                                                Columns(2);
                                                                    SetColumnWidth(-1,(getSize().x - 38) * 0.75f);
                                                                    InputText("String", (char*)var->GetData()->Ptr(), sz); validTypeIndices.push_back(0);
                                                                    if(sz == 1) {
                                                                        Checkbox("boolean", (bool*)ptr); validTypeIndices.push_back(1);
                                                                    } else if(sz == 4) {
                                                                        DragInt("int", (int*)ptr); validTypeIndices.push_back(2);
                                                                        DragFloat("float", (float*)ptr); validTypeIndices.push_back(3);
                                                                    } else if(sz == 8) {
                                                                        DragFloat2("vec2", (float*)ptr); validTypeIndices.push_back(4);
                                                                        DragInt2("ivec2", (int*)ptr); validTypeIndices.push_back(5);
                                                                    } else if(sz == 12) {
                                                                        DragFloat3("vec3", (float*)ptr); validTypeIndices.push_back(6);
                                                                        DragInt3("ivec3", (int*)ptr); validTypeIndices.push_back(7);
                                                                        ColorEdit3("rgb", (float*)ptr); validTypeIndices.push_back(8);
                                                                    } else if(sz == 16) {
                                                                        DragFloat4("vec4", (float*)ptr); validTypeIndices.push_back(9);
                                                                        DragInt4("ivec4", (int*)ptr); validTypeIndices.push_back(10);
                                                                        ColorEdit4("rgba", (float*)ptr); validTypeIndices.push_back(11);
                                                                    }
                                                                NextColumn();
                                                                    SetColumnWidth(-1,(getSize().x - 38) * 0.25f);
                                                                    for(size_t i = 0;i < validTypeIndices.size();i++) {
                                                                        const char* typeName = types[validTypeIndices[i]];
                                                                        if(Button((string("use ") + typeName).c_str(), ImVec2((getSize().x - 38) * 0.25f,22.0f))) {
                                                                            printf("using input type %s for %s\n", typeName, name.c_str());
                                                                            set_actor_var_type_event evt(name, typeName);
                                                                            m_app->onEvent(&evt);
                                                                        }
                                                                    }
                                                                EndColumns();
                                                            } else {
                                                                Text("Value too large for input");
                                                            }
                                                        EndChild();
                                                    }
                                                }
                                            }
                                        EndChild();
                                    }
                                    
                                    bool knwOpen = CollapsingHeader("Known Types", ImGuiTreeNodeFlags_Framed);
                                    SameLine(116);
                                    p = GetCursorPos();
                                    SetCursorPos(ImVec2(p.x, p.y + 2));
                                    Text("(%d)", knownCount);
                                    if(knwOpen) {
                                        BeginChild("av_sec_knw", ImVec2((getSize().x - 15), 400), true);
                                            ImVec2 kpos = GetWindowPos();
                                            SetWindowPos(ImVec2(kpos.x + 10,kpos.y));
                                            for(size_t i = 0;i < vars->GetBlockCount();i++) {
                                                Block* var = vars->GetBlock(i);
                                                
                                                string name = var->GetTypeString();
                                                string set_type = m_app->get_actor_var_type(name);
                                                
                                                ByteStream* data = var->GetData();
                                                void* ptr = data->Ptr();
                                                size_t sz = data->GetSize();
                                                if(set_type != "") {
                                                    //this AV type has been defined
                                                    PushItemWidth(160);
                                                    if(set_type == "string") InputText(name.c_str(), (char*)var->GetData()->Ptr(), sz);
                                                    else if(set_type == "bool") Checkbox(name.c_str(), (bool*)ptr);
                                                    else if(set_type == "int") DragInt(name.c_str(), (int*)ptr);
                                                    else if(set_type == "float") DragFloat(name.c_str(), (float*)ptr);
                                                    else if(set_type == "vec2") DragFloat2(name.c_str(), (float*)ptr);
                                                    else if(set_type == "ivec2") DragInt2(name.c_str(), (int*)ptr);
                                                    else if(set_type == "vec3") DragFloat3(name.c_str(), (float*)ptr);
                                                    else if(set_type == "ivec3") DragInt3(name.c_str(), (int*)ptr);
                                                    else if(set_type == "rgb") ColorEdit3(name.c_str(), (float*)ptr);
                                                    else if(set_type == "vec4") DragFloat4(name.c_str(), (float*)ptr);
                                                    else if(set_type == "ivec4") DragInt4(name.c_str(), (int*)ptr);
                                                    else if(set_type == "rgba") ColorEdit4(name.c_str(), (float*)ptr);
                                                    PopItemWidth();
                                                }
                                            }
                                        EndChild();
                                    }
                                } else {
                                    Text("No Variables Set");
                                }
                            } else {
                                //level
                                Text("No data to show yet");
                            }
                        } else {
                            Text("No actor selected");
                        }
                    EndChild();
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
