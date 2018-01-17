#include <app.h>
#include <gui/ui.h>
#include <gui/main_window.h>
#include <gui/sidebar.h>
#include <imgui_internal.h>
#include <logger.h>

namespace t4editor {
    bool use_btn(const string& type, const string& actorname, const string& varname, const string& pre, application* app) {
        if(Button(("use " + type + "##" + actorname + pre + "_unk_av_" + varname).c_str(), ImVec2(GetColumnWidth() - 7.0f, 20.0f))) {
            printf("using input type %s for %s\n", type.c_str(), varname.c_str());
            set_actor_var_type_event evt(varname, type);
            app->onEvent(&evt);
			return true;
        }
		return false;
    }
    void sidebar::renderLocalActorVariables() {
        int count = 0;
        int knownCount = 0;
        int unknownCount = 0;
        if(Actor && Actor->actorTraits && Actor->actorTraits->localVariables()) {
            ActorVariables* vars = Actor->actorTraits->localVariables();
            count = vars->GetBlockCount();
            for(size_t i = 0;i < vars->GetBlockCount();i++) {
                Block* var = vars->GetBlock(i);

                string name = var->GetTypeString();
                string set_type = m_app->get_actor_var_type(name);
                if(set_type == "") unknownCount++;
                else knownCount++;
            }
        }
        
        bool varsOpen = CollapsingHeader("Actor Variables (local)", ImGuiTreeNodeFlags_Framed);
        SameLine(165);
        ImVec2 p = GetCursorPos();
        SetCursorPos(ImVec2(p.x, p.y + 2));
        Text("(%d)", count);
        if(varsOpen) {
            Indent(10.0f);
                if(Actor) {
                    if(Actor->actorTraits) {
                        //actor instance
                        ActorVariables* vars = Actor->actorTraits->localVariables();
                        
                        if(vars) {
							bool unkOpen = CollapsingHeader("Unknown Types##_lc_av", ImGuiTreeNodeFlags_Framed);
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
                                                
                                                string input_prefix = "##" + string(Actor->actorTraits->Name) + "_lc_unk_av_";
                                            
                                                Indent(10.0f);
                                                    Columns(2, (string(Actor->actorTraits->Name) + "_lc_unk_av").c_str());
                                                        if(sz <= 64) {
                                                            PushItemWidth(GetColumnWidth() - 10.0f);
                                                            InputText((input_prefix + name + "_String").c_str(), (char*)var->GetData()->Ptr(), sz, ImGuiInputTextFlags_ReadOnly);
                                                            if(sz == 1) {
                                                                float indent = ((GetColumnWidth() - 10.0f) / 2.0f) - 11.0f;
                                                                Indent(indent);
                                                                Checkbox((input_prefix + name + "_boolean").c_str(), (bool*)ptr);
                                                                Unindent(indent);
                                                            } else if(sz == 4) {
                                                                DragInt((input_prefix + name + "_int").c_str(), (int*)ptr);
                                                                DragFloat((input_prefix + name + "_float").c_str(), (float*)ptr);
                                                            } else if(sz == 8) {
                                                                DragFloat2((input_prefix + name + "_vec2").c_str(), (float*)ptr);
                                                                DragInt2((input_prefix + name + "_ivec2").c_str(), (int*)ptr);
                                                            } else if(sz == 12) {
                                                                DragFloat3((input_prefix + name + "_vec3").c_str(), (float*)ptr);
                                                                DragInt3((input_prefix + name + "_ivec3").c_str(), (int*)ptr);
                                                                ColorEdit3((input_prefix + name + "_rgb").c_str(), (float*)ptr);
                                                            } else if(sz == 16) {
                                                                DragFloat4((input_prefix + name + "_vec4").c_str(), (float*)ptr);
                                                                DragInt4((input_prefix + name + "_ivec4").c_str(), (int*)ptr);
                                                                ColorEdit4((input_prefix + name + "_rgba").c_str(), (float*)ptr);
                                                            }
                                                            PopItemWidth();
                                                        }
                                                        if(Button(("Raw Data##" + string(Actor->actorTraits->Name) + "_lc_unk_av_"+name).c_str(), ImVec2(GetColumnWidth() - 10.0f, 20.0f))) {
                                                            open_memory_editor_event evt("Data of " + string(Actor->actorTraits->Name) + " variable: " + name, (u8*)ptr, sz);
                                                            m_app->onEvent(&evt);
                                                        }
                                                    NextColumn();
														//If this button is clicked, ->GetData() will no longer return the correct value for this variable
                                                        if(use_btn("string", string(Actor->actorTraits->Name), name, "_lc", m_app)) var->useUIBuf();

                                                        if(sz == 1) {
                                                            use_btn("boolean", string(Actor->actorTraits->Name), "_lc", name, m_app);
                                                        } else if(sz == 4) {
                                                            use_btn("int", string(Actor->actorTraits->Name), name, "_lc", m_app);
                                                            use_btn("float", string(Actor->actorTraits->Name), name, "_lc", m_app);
                                                            
                                                        } else if(sz == 8) {
                                                            use_btn("vec2", string(Actor->actorTraits->Name), name, "_lc", m_app);
                                                            use_btn("ivec2", string(Actor->actorTraits->Name), name, "_lc", m_app);
                                                        } else if(sz == 12) {
                                                            use_btn("vec3", string(Actor->actorTraits->Name), name, "_lc", m_app);
                                                            use_btn("ivec3", string(Actor->actorTraits->Name), name, "_lc", m_app);
                                                            use_btn("rgb", string(Actor->actorTraits->Name), name, "_lc", m_app);
                                                        } else if(sz == 16) {
                                                            use_btn("vec4", string(Actor->actorTraits->Name), name, "_lc", m_app);
                                                            use_btn("ivec4", string(Actor->actorTraits->Name), name, "_lc", m_app);
                                                            use_btn("rgba", string(Actor->actorTraits->Name), name, "_lc", m_app);
                                                        }
                                                        use_btn("data", string(Actor->actorTraits->Name), name, "_lc", m_app);
                                                    EndColumns();
                                                Unindent(10.0f);
                                            }
                                            ImVec2 cp = GetCursorPos();
                                            SetCursorPos(ImVec2(cp.x, cp.y + 3));
                                        }
                                    }
                                Unindent(10.0f);
                            }
                            
							bool knwOpen = CollapsingHeader("Known Types##_lc_av", ImGuiTreeNodeFlags_Framed);
                            SameLine(122);
                            p = GetCursorPos();
                            SetCursorPos(ImVec2(p.x, p.y + 2));
                            Text("(%d)", knownCount);
                            if(knwOpen) {
                                Indent(10.0f);
                                    Columns(2, (string(Actor->actorTraits->Name) + "_lc_knw_av").c_str());
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
                                                name = "##"+string(Actor->actorTraits->Name)+"_lc_knw_"+name;
                                                if(set_type == "string") {
													var->useUIBuf(); //calling this has no effect after the first time it's called on the block
													InputText(name.c_str(), var->uiBuf(), UI_BUFFER_SIZE);
												}
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
                                                else if(set_type == "data") {
                                                    if(Button(("Raw Data##" + string(Actor->actorTraits->Name) + "_lc_knw_av_"+name).c_str(), ImVec2(GetColumnWidth() - 10.0f, 20.0f))) {
                                                        open_memory_editor_event evt("Data of " + string(Actor->actorTraits->Name) + " variable: " + var->GetTypeString(), (u8*)ptr, sz);
                                                        m_app->onEvent(&evt);
                                                    }
                                                }
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
    void sidebar::renderGlobalActorVariables() {
        int count = 0;
        int knownCount = 0;
        int unknownCount = 0;
        if(Actor && Actor->actorTraits && Actor->actorTraits->globalVariables()) {
            ActorVariables* vars = Actor->actorTraits->globalVariables();
            count = vars->GetBlockCount();
            for(size_t i = 0;i < vars->GetBlockCount();i++) {
                Block* var = vars->GetBlock(i);

                string name = var->GetTypeString();
                string set_type = m_app->get_actor_var_type(name);
                if(set_type == "") unknownCount++;
                else knownCount++;
            }
        }
        bool varsOpen = CollapsingHeader("Actor Variables (global)", ImGuiTreeNodeFlags_Framed);
        SameLine(174);
        ImVec2 p = GetCursorPos();
        SetCursorPos(ImVec2(p.x, p.y + 2));
        Text("(%d)", count);
        if(varsOpen) {
            Indent(10.0f);
                if(Actor) {
                    if(Actor->actorTraits) {
                        //actor instance
                        ActorVariables* vars = Actor->actorTraits->globalVariables();
                        
                        if(vars) {
							bool unkOpen = CollapsingHeader("Unknown Types##_gb_unk_av", ImGuiTreeNodeFlags_Framed);
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
                                                
                                                string input_prefix = "##" + string(Actor->actorTraits->Name) + "_gb_unk_av_";
                                            
                                                Indent(10.0f);
                                                    Columns(3, (string(Actor->actorTraits->Name) + "_gb_unk_av").c_str());
                                                        if(sz <= 64) {
                                                            PushItemWidth(GetColumnWidth() - 10.0f);
                                                            InputText((input_prefix + name + "_String").c_str(), (char*)var->GetData()->Ptr(), sz, ImGuiInputTextFlags_ReadOnly);
                                                            if(sz == 1) {
                                                                float indent = ((GetColumnWidth() - 10.0f) / 2.0f) - 11.0f;
                                                                Indent(indent);
                                                                Checkbox((input_prefix + name + "_boolean").c_str(), (bool*)ptr);
                                                                Unindent(indent);
                                                            } else if(sz == 4) {
                                                                DragInt((input_prefix + name + "_int").c_str(), (int*)ptr);
                                                                DragFloat((input_prefix + name + "_float").c_str(), (float*)ptr);
                                                            } else if(sz == 8) {
                                                                DragFloat2((input_prefix + name + "_vec2").c_str(), (float*)ptr);
                                                                DragInt2((input_prefix + name + "_ivec2").c_str(), (int*)ptr);
                                                            } else if(sz == 12) {
                                                                DragFloat3((input_prefix + name + "_vec3").c_str(), (float*)ptr);
                                                                DragInt3((input_prefix + name + "_ivec3").c_str(), (int*)ptr);
                                                                ColorEdit3((input_prefix + name + "_rgb").c_str(), (float*)ptr);
                                                            } else if(sz == 16) {
                                                                DragFloat4((input_prefix + name + "_vec4").c_str(), (float*)ptr);
                                                                DragInt4((input_prefix + name + "_ivec4").c_str(), (int*)ptr);
                                                                ColorEdit4((input_prefix + name + "_rgba").c_str(), (float*)ptr);
                                                            }
                                                            PopItemWidth();
                                                        }
                                                        if(Button(("Raw Data##" + string(Actor->actorTraits->Name) + "_gb_unk_av_"+name).c_str(), ImVec2(GetColumnWidth() - 10.0f, 20.0f))) {
                                                            open_memory_editor_event evt("Data of " + string(Actor->actorTraits->Name) + " variable: " + name, (u8*)ptr, sz);
                                                            m_app->onEvent(&evt);
                                                        }
                                                    NextColumn();
														//If this button is clicked, ->GetData() will no longer return the correct value for this variable
                                                        if(use_btn("string", string(Actor->actorTraits->Name), name, "_lc", m_app)) var->useUIBuf();

                                                        if(sz == 1) {
                                                            use_btn("boolean", string(Actor->actorTraits->Name), name, "_lc", m_app);
                                                        } else if(sz == 4) {
                                                            use_btn("int", string(Actor->actorTraits->Name), name, "_lc", m_app);
                                                            use_btn("float", string(Actor->actorTraits->Name), name, "_lc", m_app);
                                                            
                                                        } else if(sz == 8) {
                                                            use_btn("vec2", string(Actor->actorTraits->Name), name, "_lc", m_app);
                                                            use_btn("ivec2", string(Actor->actorTraits->Name), name, "_lc", m_app);
                                                        } else if(sz == 12) {
                                                            use_btn("vec3", string(Actor->actorTraits->Name), name, "_lc", m_app);
                                                            use_btn("ivec3", string(Actor->actorTraits->Name), name, "_lc", m_app);
                                                            use_btn("rgb", string(Actor->actorTraits->Name), name, "_lc", m_app);
                                                        } else if(sz == 16) {
                                                            use_btn("vec4", string(Actor->actorTraits->Name), name, "_lc", m_app);
                                                            use_btn("ivec4", string(Actor->actorTraits->Name), name, "_lc", m_app);
                                                            use_btn("rgba", string(Actor->actorTraits->Name), name, "_lc", m_app);
                                                        }
                                                        use_btn("data", string(Actor->actorTraits->Name), name, "_lc", m_app);
													NextColumn();
														SetColumnWidth(-1,35.0f);
                                                        PushItemWidth(GetColumnWidth() - 10.0f);
															ActorVariables* local = Actor->actorTraits->localVariables();
															//Make sure the variable doesn't already exist locally
															bool found = false;
															for(int i = 0;i < local->GetBlockCount();i++) {
																if(local->GetBlock(i)->GetTypeString() == name) {
																	found = true;
																	break;
																}
															}
															if(!found) {
																if(Button(("Add##_add_gb_unk_av_" + name).c_str())) local->AddBlock(var);
															}
														PopItemWidth();
                                                    EndColumns();
                                                Unindent(10.0f);
                                            }
                                            ImVec2 cp = GetCursorPos();
                                            SetCursorPos(ImVec2(cp.x, cp.y + 3));
                                        }
                                    }
                                Unindent(10.0f);
                            }
                            
							bool knwOpen = CollapsingHeader("Known Types##_gb_knw_av", ImGuiTreeNodeFlags_Framed);
                            SameLine(122);
                            p = GetCursorPos();
                            SetCursorPos(ImVec2(p.x, p.y + 2));
                            Text("(%d)", knownCount);
                            if(knwOpen) {
                                Indent(10.0f);
                                    Columns(3, (string(Actor->actorTraits->Name) + "_gb_knw_av").c_str());
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
                                                name = "##"+string(Actor->actorTraits->Name)+"_gb_knw_"+name;
                                                if(set_type == "string") InputText(name.c_str(), var->uiBuf(), UI_BUFFER_SIZE, ImGuiInputTextFlags_ReadOnly);
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
                                                else if(set_type == "data") {
                                                    if(Button(("Raw Data##" + string(Actor->actorTraits->Name) + "_gb_knw_av_"+name).c_str(), ImVec2(GetColumnWidth() - 10.0f, 20.0f))) {
                                                        open_memory_editor_event evt("Data of " + string(Actor->actorTraits->Name) + " variable: " + var->GetTypeString(), (u8*)ptr, sz);
                                                        m_app->onEvent(&evt);
                                                    }
                                                }
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
									NextColumn();
										SetColumnWidth(-1,35.0f);
										PushItemWidth(GetColumnWidth() - 10.0f);
                                        for(size_t i = 0;i < vars->GetBlockCount();i++) {
                                            Block* var = vars->GetBlock(i);
                                            
                                            string name = var->GetTypeString();
                                            string set_type = m_app->get_actor_var_type(name);
                                            
                                            if(set_type != "") {
												ActorVariables* local = Actor->actorTraits->localVariables();
												//Make sure the variable doesn't already exist locally
												bool found = false;
												for(int i = 0;i < local->GetBlockCount();i++) {
													if(local->GetBlock(i)->GetTypeString() == name) {
														found = true;
														break;
													}
												}
												if(!found) {
													if(Button(("Add##_add_gb_knw_av_" + name).c_str())) local->AddBlock(var);
												} else {
													ImVec2 cp = GetCursorPos();
													SetCursorPos(ImVec2(cp.x, cp.y + 20.0f));
												}
                                            }
                                        }
										PopItemWidth();
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
