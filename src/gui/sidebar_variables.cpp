#include <app.h>
#include <gui/ui.h>
#include <gui/main_window.h>
#include <gui/sidebar.h>
#include <imgui_internal.h>

namespace t4editor {
    void sidebar::renderActorVariables() {
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
            Indent(10.0f);
                if(Actor) {
                    if(Actor->actorTraits) {
                        //actor instance
                        ActorVariables* vars = Actor->actorTraits->variables();
                        
                        if(vars) {
                            bool unkOpen = CollapsingHeader("Unknown Types", ImGuiTreeNodeFlags_Framed);
                            SameLine(136);
                            p = GetCursorPos();
                            SetCursorPos(ImVec2(p.x, p.y + 2));
                            Text("(%d)", unknownCount);
                            if(unkOpen) {
                                Indent(10.0f);
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
                                                float ht = 110.0f;
                                                if(sz == 1) ht = 66.0f;
                                                else if(sz == 4) ht = 88.0f;
                                                else if(sz == 8) ht = 88.0f;
                                                else if(sz == 12) ht = 110.0f;
                                                else if(sz == 16)  ht = 110.0f;
                                                else ht = 44.0f;
                                                
                                                if(sz > 64) ht = 66.0f;
                                                
                                                string input_prefix = "##" + Actor->actorTraits->Name + "_unk_av_";
                                            
                                                Indent(10.0f);
                                                    if(sz <= 64) {
                                                        vector<int> validTypeIndices;
                                                        Columns(2, (Actor->actorTraits->Name + "_unk_av").c_str());
                                                            PushItemWidth(GetColumnWidth() - 10.0f);
                                                            InputText((input_prefix + "String").c_str(), (char*)var->GetData()->Ptr(), sz, ImGuiInputTextFlags_ReadOnly); validTypeIndices.push_back(0);
                                                            if(sz == 1) {
                                                                Checkbox((input_prefix + "boolean").c_str(), (bool*)ptr); validTypeIndices.push_back(1);
                                                            } else if(sz == 4) {
                                                                DragInt((input_prefix + "int").c_str(), (int*)ptr); validTypeIndices.push_back(2);
                                                                DragFloat((input_prefix + "float").c_str(), (float*)ptr); validTypeIndices.push_back(3);
                                                            } else if(sz == 8) {
                                                                DragFloat2((input_prefix + "vec2").c_str(), (float*)ptr); validTypeIndices.push_back(4);
                                                                DragInt2((input_prefix + "ivec2").c_str(), (int*)ptr); validTypeIndices.push_back(5);
                                                            } else if(sz == 12) {
                                                                DragFloat3((input_prefix + "vec3").c_str(), (float*)ptr); validTypeIndices.push_back(6);
                                                                DragInt3((input_prefix + "ivec3").c_str(), (int*)ptr); validTypeIndices.push_back(7);
                                                                ColorEdit3((input_prefix + "rgb").c_str(), (float*)ptr); validTypeIndices.push_back(8);
                                                            } else if(sz == 16) {
                                                                DragFloat4((input_prefix + "vec4").c_str(), (float*)ptr); validTypeIndices.push_back(9);
                                                                DragInt4((input_prefix + "ivec4").c_str(), (int*)ptr); validTypeIndices.push_back(10);
                                                                ColorEdit4((input_prefix + "rgba").c_str(), (float*)ptr); validTypeIndices.push_back(11);
                                                            }
                                                            PopItemWidth();
                                                        NextColumn();
                                                            for(size_t i = 0;i < validTypeIndices.size();i++) {
                                                                const char* typeName = types[validTypeIndices[i]];
                                                                if(Button((string("use ") + typeName).c_str(), ImVec2(GetColumnWidth() - 10.0f, 22.0f))) {
                                                                    printf("using input type %s for %s\n", typeName, name.c_str());
                                                                    set_actor_var_type_event evt(name, typeName);
                                                                    m_app->onEvent(&evt);
                                                                }
                                                            }
                                                        EndColumns();
                                                    } else {
                                                        Text("Value too large for input");
                                                    }
                                                    ImVec2 cp = GetCursorPos();
                                                    SetCursorPos(ImVec2(cp.x, cp.y + 3));
                                                    if(Button("View data in hex editor", ImVec2(getSize().x - 30.0f, 22.0f))) {
                                                        open_memory_editor_event evt("Data of " + Actor->actorTraits->Name + " variable: " + name, (u8*)ptr, sz);
                                                        m_app->onEvent(&evt);
                                                    }
                                                Unindent(10.0f);
                                            }
                                            ImVec2 cp = GetCursorPos();
                                            SetCursorPos(ImVec2(cp.x, cp.y + 3));
                                        }
                                    }
                                Unindent(10.0f);
                            }
                            
                            bool knwOpen = CollapsingHeader("Known Types", ImGuiTreeNodeFlags_Framed);
                            SameLine(122);
                            p = GetCursorPos();
                            SetCursorPos(ImVec2(p.x, p.y + 2));
                            Text("(%d)", knownCount);
                            if(knwOpen) {
                                Indent(10.0f);
                                    Columns(2, (Actor->actorTraits->Name + "_knw_av").c_str());
                                        float col_pad = 10.0f;
                                        for(size_t i = 0;i < vars->GetBlockCount();i++) {
                                            Block* var = vars->GetBlock(i);
                                            
                                            string name = var->GetTypeString();
                                            string set_type = m_app->get_actor_var_type(name);
                                            
                                            ByteStream* data = var->GetData();
                                            void* ptr = data->Ptr();
                                            size_t sz = data->GetSize();
                                            if(set_type != "") {
                                                //this AV type has been defined
                                                PushItemWidth(GetColumnWidth() - col_pad);
                                                name = "##"+Actor->actorTraits->Name+"_knw_"+name;
                                                if(set_type == "string") InputText(name.c_str(), (char*)var->GetData()->Ptr(), sz, ImGuiInputTextFlags_ReadOnly);
                                                else if(set_type == "bool") {
                                                    float indent = ((GetColumnWidth() - col_pad) / 2.0f) - 11.0f;
                                                    Indent(indent);
                                                    Checkbox(name.c_str(), (bool*)ptr);
                                                    Unindent(indent);
                                                }
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
                                    NextColumn();
                                        for(size_t i = 0;i < vars->GetBlockCount();i++) {
                                            Block* var = vars->GetBlock(i);
                                            
                                            string name = var->GetTypeString();
                                            string set_type = m_app->get_actor_var_type(name);
                                            
                                            if(set_type != "") {
                                                ImVec2 cp = GetCursorPos();
                                                SetCursorPos(ImVec2(cp.x, cp.y + 2.0f));
                                                Text("%s", name.c_str());
                                                cp = GetCursorPos();
                                                SetCursorPos(ImVec2(cp.x, cp.y + 2.0f));
                                            }
                                        }
                                    EndColumns();
                                Unindent(10.0f);
                            }
                        } else {
                            Text("No Variables Set");
                        }
                    } else {
                        //level
                        Text("Level actors have no actor variables");
                    }
                } else {
                    Text("No actor selected");
                }
            Unindent(10.0f);
        }
    }
};
