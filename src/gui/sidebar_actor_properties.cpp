#include <app.h>
#include <gui/ui.h>
#include <gui/main_window.h>
#include <gui/sidebar.h>
#include <imgui_internal.h>

namespace t4editor {
    inline bool use_btn(const string& type, const string& entityPath, const string& blockname, application* app) {
        if(Button(("use " + type + "##" + entityPath + "_unk_av_" + blockname).c_str(), ImVec2(GetColumnWidth() - 7.0f, 20.0f))) {
            printf("using input type %s for %s\n", type.c_str(), blockname.c_str());
            set_actor_block_type_event evt(blockname, type);
            app->onEvent(&evt);
			return true;
        }
		return false;
    }
    void renderBlock(Block* b, const string& bPath, application* app, const string& entityName);
    void renderBlockData(Block* b, ByteStream* data, const string& bName, const string& bPath, application* app, const string& entityName);
    void sidebar::renderActorData() {
        ATRFile* relatedActor = 0;
        string entityName;
        if(Actor || Level) {
            if(Actor->actorTraits) {
                entityName = Actor->actorTraits->Name;
                relatedActor = Actor->actorTraits->Actor->GetATR();
            } else {
                string file = TransformPseudoPathToRealPath(Level->levelFile()->GetActorMeshFile());
                int last_slash = file.find_last_of("/");
                entityName = file.substr(last_slash + 1, file.length() - (last_slash + 1));
                relatedActor = Level->levelFile();
            }
        }
        
        if(CollapsingHeader("Actor Properties")) {
            if(!relatedActor) {
                Text("No actor selected");
                return;
            }
            
            renderBlock(relatedActor->GetRootBlock(), entityName + "_" + relatedActor->GetRootBlock()->GetTypeString(), m_app, entityName);
        }
    }
    
    //aaah, recursion... my old friend
    void renderBlock(Block* b, const string& bPath, application* app, const string& entityName) {
        Indent(10.0f);
            if(!b) Text("No data. Please report this with a screenshot");
            else {
                if(CollapsingHeader(b->GetTypeString().c_str())) {
                    Indent(10.0f);
                        if(b->GetChildCount() != 0) {
                            for(size_t i = 0;i < b->GetChildCount();i++) {
                                renderBlock(b->GetChild(i), bPath + "_" + b->GetChild(i)->GetTypeString(), app, entityName);
                            }
                        } else renderBlockData(b, b->GetData(), b->GetTypeString(), bPath, app, entityName);
                    Unindent(10.0f);
                }
            }
        Unindent(10.0f);
    }
    void renderBlockData(Block* b, ByteStream* data, const string& bName, const string& bPath, application* app, const string& entityName) {
        string name = bName;
        string set_type = app->get_actor_block_type(name);
        void* ptr = data->Ptr();
        size_t sz = data->GetSize();
        if(set_type == "") {
            //AP has unknown type, present options
            float ht = 110.0f;
            if(sz == 1) ht = 66.0f;
            else if(sz == 4) ht = 88.0f;
            else if(sz == 8) ht = 88.0f;
            else if(sz == 12) ht = 110.0f;
            else if(sz == 16)  ht = 110.0f;
            else ht = 44.0f;
            
            if(sz > 64) ht = 66.0f;
            
            string input_prefix = "##" + bPath;
        
            Indent(10.0f);
                Columns(2, (bPath + "_unk_ap").c_str());
                    if(sz <= 64) {
                        PushItemWidth(GetColumnWidth() - 10.0f);
                        InputText((input_prefix + name + "_String").c_str(), (char*)ptr, sz, ImGuiInputTextFlags_ReadOnly);
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
                    if(Button(("Raw Data##" + bPath + "_unk_ab_" + name).c_str(), ImVec2(GetColumnWidth() - 10.0f, 20.0f))) {
                        open_memory_editor_event evt("Data of " + entityName + " block: " + name, (u8*)ptr, sz);
                        app->onEvent(&evt);
                    }
                NextColumn();
					//If this button is clicked, ->GetData() will no longer return the correct value for this variable
                    if(use_btn("string", bPath, name, app)) b->useUIBuf();
                    if(sz == 1) {
                        use_btn("boolean", bPath, name, app);
                    } else if(sz == 4) {
                        use_btn("int", bPath, name, app);
                        use_btn("float", bPath, name, app);
                        
                    } else if(sz == 8) {
                        use_btn("vec2", bPath, name, app);
                        use_btn("ivec2", bPath, name, app);
                    } else if(sz == 12) {
                        use_btn("vec3", bPath, name, app);
                        use_btn("ivec3", bPath, name, app);
                        use_btn("rgb", bPath, name, app);
                    } else if(sz == 16) {
                        use_btn("vec4", bPath, name, app);
                        use_btn("ivec4", bPath, name, app);
                        use_btn("rgba", bPath, name, app);
                    }
                    use_btn("data", bPath, name, app);
                EndColumns();
            Unindent(10.0f);
            ImVec2 cp = GetCursorPos();
            SetCursorPos(ImVec2(cp.x, cp.y + 3));
        } else {
            //this AB type has been defined
            Indent(10.0f);
                name = "##" + bPath + "_knw_" + name;
                if(set_type == "string") {
					b->useUIBuf(); //calling this has no effect after the first time it's called on the block
					InputText(name.c_str(), b->uiBuf(), UI_BUFFER_SIZE);
				}
                else if(set_type == "bool") {
                    float indent = ((GetWindowSize().x - 10.0f) / 2.0f) - 11.0f;
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
                    if(Button(("Raw Data##" + bPath + "_knw_ab_" + name).c_str(), ImVec2(GetColumnWidth() - 10.0f, 20.0f))) {
                        open_memory_editor_event evt("Data of " + entityName + " block: " + name, (u8*)ptr, sz);
                        app->onEvent(&evt);
                    }
                }
            Unindent(10.0f);
        }
    }
}
