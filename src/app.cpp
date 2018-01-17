#include <app.h>
#include <stdio.h>
#include <stdlib.h>
#include <imgui.h>
#include <gui/panel.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <algorithm>
#include <cassert>
#include <logger.h>

#include <render/SOIL/SOIL.h>
#include <util/json.hpp>
#include <iostream>
#include <fstream>


#define DEBUG_DRAW_IMPLEMENTATION
#include <render/debug_draw.hpp>

class DDRenderInterfaceNull final : public dd::RenderInterface
{
public:
    ~DDRenderInterfaceNull() { }
};

namespace t4editor {
    application::application(int argc, const char* argv[]) {
        for(int i = 0;i < argc;i++) {
            m_args.push_back(string(argv[i]));
        }
        m_windowWidth = 1200;
        m_windowHeight = 900;
        m_window = 0;
        m_fs = 0;
        m_level = 0;
        m_framebuffer = 0;
        m_windowPosX = 0;
        m_windowPosY = 150;
        m_defaultTex = 0;
        m_updatingCache = false;
        m_UseCulling = true;
        m_Camera = new Camera(90, m_windowWidth / m_windowHeight, 1.0f, 1000.0f);
		m_restoringBackup = false;
		m_viewChanged = true;
		m_actorToImport = nullptr;
		m_transformingActor = false;
		m_transformType = ImGuizmo::TRANSLATE;
		m_selectedActor.actorId = -1;
		m_actorUnderCursor.actorId = -1;
		m_cursorOverLevelView = false;
		m_isImportingActor = false;
    }
    
    application::~application() {
        vec2 wSize = m_window->getSize(false);
        vec2 wPos = m_window->getPosition();
        
        if (m_shader) delete m_shader;
        if (m_fs) delete m_fs;
        if (m_window) delete m_window;
        if (m_framebuffer) {
            for (size_t i = 0; i < m_framebuffer->attachments.size(); i++) {
                delete m_framebuffer->attachments[i];
            }
            delete m_framebuffer;
        }

        nlohmann::json j;
        j["editor_data_path"] = m_editorDataPath;
        j["game_data_path"] = m_dataPath;
        j["res_x"] = (int)wSize.x;
        j["res_y"] = (int)wSize.y;
        j["pos_x"] = (int)wPos.x;
        j["pos_y"] = (int)wPos.y;
        for (auto i = m_actor_var_types.begin(); i != m_actor_var_types.end(); i++) {
            string key = "av__" + i->first;
            j[key.c_str()] = i->second.c_str();
        }

        for (auto i = m_actor_block_types.begin(); i != m_actor_block_types.end(); i++) {
            string key = "ab__" + i->first;
            j[key.c_str()] = i->second.c_str();
        }

        string path = string(m_args[0]);
        size_t lidx = path.find_last_of('\\');
        if(lidx == path.npos) lidx = path.find_last_of('/');
        path = path.substr(0, lidx);

        printf("Writing json data: %s\n", j.dump().c_str());
        std::ofstream o(path + '/' + "config.json");
        o << std::setw(4) << j << std::endl;
        o.close();

        if (m_level) delete m_level;
    }

    bool application::initialize() {
        if (!load_config()) return false;
        m_window = new window(this, m_windowWidth, m_windowHeight);
        if (!m_window->isOpen()) {
            return false;
        }

        glfwSetWindowPos(m_window->getWindow(), m_windowPosX, m_windowPosY);

        m_fs = new turokfs(m_dataPath);

        m_shader = new shader(this);
        m_shader->attribute("position", 0);
        m_shader->attribute("normal", 1);
        m_shader->attribute("texc", 2);
        m_shader->loadFromFile("shaders/simple.vsh", "shaders/simple.fsh");

        vec2 dims = m_window->getSize();
        m_framebuffer = new framebuffer(dims.x * 0.75f, dims.y * 0.7f, true);
        m_framebuffer->attachments.push_back(new texture(dims.x, dims.y, GL_RGB, GL_UNSIGNED_BYTE, true)); //color buffer
        m_framebuffer->attachments.push_back(new texture(dims.x, dims.y, GL_RGB, GL_UNSIGNED_BYTE, true)); //asset_id
        m_framebuffer->attachments.push_back(new texture(dims.x, dims.y, GL_RGB, GL_UNSIGNED_BYTE, true)); //asset_submesh_id
        m_framebuffer->attachments.push_back(new texture(dims.x, dims.y, GL_RGB, GL_UNSIGNED_BYTE, true)); //asset_submesh_chunk_id

        u32* defaultTexData = new u32[2 * 2];
        for (u8 x = 0; x < 2; x++) {
            for (u8 y = 0; y < 2; y++) {
                u8* pixel = (u8*)(&defaultTexData[x + (y * 2)]);
                if ((x + (y * 2)) % 2 == 0) {
                    pixel[0] = 100;
                    pixel[1] = 20;
                    pixel[2] = 200;
                    pixel[3] = 255;
                }
                else {
                    pixel[0] = 210;
                    pixel[1] = 210;
                    pixel[2] = 210;
                    pixel[3] = 210;
                }
            }
        }
        m_defaultTex = new texture(2, 2, GL_RGBA, GL_UNSIGNED_BYTE, false, (u8*)defaultTexData);
        delete[] defaultTexData;
        m_defaultTex->bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);

        return true;
    }

    bool application::load_config() {
        nlohmann::json j;
        std::ifstream i("config.json");
        i >> j;

        for (nlohmann::json::iterator it = j.begin(); it != j.end(); it++) {
            std::string name = it.key();
            if (name == "game_data_path") {
                m_dataPath = it.value().get<string>();
                SetTurokDirectory(m_dataPath);
                printf("Using '%s' as game data directory\n", m_dataPath.c_str());
            }
            else if (name == "editor_data_path") {
                m_editorDataPath = it.value().get<string>();
                printf("Using '%s' as editor data directory\n", m_editorDataPath.c_str());
            }
            else if (name == "res_x") m_windowWidth = it.value().get<int>();
            else if (name == "res_y") m_windowHeight = it.value().get<int>();
            else if (name == "pos_x") m_windowPosX = it.value().get<int>();
            else if (name == "pos_y") m_windowPosY = it.value().get<int>();
            else if (name.find("av__") != name.npos) {
                string vName = name.substr(4, name.length() - 4);
                define_actor_var_type(vName, it.value());
            }
            else if (name.find("ab__") != name.npos) {
                string bName = name.substr(4, name.length() - 4);
                define_actor_block_type(bName, it.value());
            }
        }

        i.close();
        return true;
    }

    void application::add_panel(ui_panel *panel) {
        m_panels.push_back(panel);
        panel->m_app = this;
        panel->onAttach(nullptr);
    }
    
    void application::remove_panel(ui_panel *panel) {
        for(auto i = m_panels.begin();i != m_panels.end();i++) {
            if((*i) == panel) {
                m_panels.erase(i);
                panel->m_app = nullptr;
                return;
            }
        }
    }
    
    void application::onEvent(event *e) {
        if(e->name == "load_level") m_levelToLoad = ((load_level_event*)e)->path;
		else if(e->name == "test_level") test_level();
        else if(e->name == "input_event") {
            input_event* input = (input_event*)e;

            switch(input->type) {
                case input_event::ET_MOUSE_MOVE:
                    break;
                case input_event::ET_MOUSE_LEFT_DOWN:
                    break;
                case input_event::ET_MOUSE_LEFT_UP: {
					if(m_actorToImport && m_cursorOverLevelView) {
						ActorDef* def = m_level->levelFile()->GetActors()->InstantiateActor(m_actorToImport->actorFile);
						def->Position.x = m_actorImportPos.x;
						def->Position.y = m_actorImportPos.y;
						def->Position.z = m_actorImportPos.z;
						actor* a = m_level->actor_added();
						m_selectedActor.actorId = a->editor_id;
						actor_selection_event evt(m_level, a, nullptr, a->editor_id, -1, -1, false);
						onEvent(&evt);
					}
					m_actorToImport = nullptr;
					m_viewChanged = true;
                    break;
                }
				case input_event::ET_MOUSE_RIGHT_DOWN:
					m_actorToImport = nullptr;
					break;
				case input_event::ET_KEY_UP: {
					if (input->key == GLFW_KEY_P) {
						m_Camera->ToggleUpdateFrustum();
						printf("Toggling frustum updating: %s", m_Camera->m_UpdateFrustum ? "True" : "False");
					} else if (input->key == GLFW_KEY_O) {
						ToggleCulling();
						printf("Toggling culling usage: %s", m_UseCulling ? "True" : "False");
					}
				}
			}
        }
        else if(e->name == "window_resize") {
            //app_resize_event* evt = (app_resize_event*)e;
        }
		else if(e->name == "define_av_type") {
            set_actor_var_type_event* evt = (set_actor_var_type_event*)e;
            define_actor_var_type(evt->vname, evt->vtype);
        }
		else if(e->name == "define_ab_type") {
            set_actor_block_type_event* evt = (set_actor_block_type_event*)e;
            define_actor_block_type(evt->bname, evt->btype);
        }
		else if(e->name == "actor_added") {
			m_level->actor_added();
		}
		else if(e->name == "actor_deleted") {
			m_level->actor_deleted((actor_deleted_event*)e);
			m_viewChanged = true;
		}
		else if(e->name == "save_level") {
			if(m_level) m_level->levelFile()->Save(m_dataPath + "/" + m_level->levelFile()->GetFileName());
			else printf("No level loaded\n");
		}
		else if(e->name == "update_actor_cache") {
			if(!m_updatingCache && !m_restoringBackup) {
				update_actor_cache();
				m_updatingCache = true;
				m_cacheProgress = 0.0f;
			}
		}
		else if(e->name == "restore_backups") {
			if(!m_updatingCache && !m_restoringBackup) {
				restore_backups();
				m_restoringBackup = true;
				m_restoreProgress = 0.0f;
			}
		}
		else if(e->name == "actor_selection") {
			trigger_repaint();
		}
		else if(e->name == "begin_actor_import") {
			import_actor_begin_event* evt = (import_actor_begin_event*)e;
			m_actorToImport = evt->actorToImport;
		}
        
        for(auto i = m_panels.begin();i != m_panels.end();i++) {
            (*i)->onEvent(e);
        }
        
        m_window->onEvent(e);
    }
    
    void application::load_level(const string &path) {
        if(m_level) {
            delete m_level;
            dispatchNamedEvent("level_unloaded");
        }
        
        m_level = new level(this);
        
        if(!m_level->load(path)) {
            delete m_level;
            m_level = 0;
        }
        dispatchNamedEvent("level_loaded");
        m_viewChanged = true;
        return;
    }

	void application::test_level() {
		if(!m_level) return;
		
		if(m_level->levelFile()->Save(m_dataPath + "/data/levels/u_jcowlishaw/screens/BootUpSequence/bootupsequence.atr")) {
			monitor_testing(m_dataPath + "/Turok4.exe");
		} else {
			printf("Failed to save current level for testing\n");
		}
	}

	void application::level_test_done() {
		m_level->levelFile()->Restore();

		printf("Restoring previous bootupsequence.atr\n");
		string backup = m_dataPath + "/data/levels/u_jcowlishaw/screens/BootUpSequence/bootupsequence.atr.editorbak";
		string original = m_dataPath + "/data/levels/u_jcowlishaw/screens/BootUpSequence/bootupsequence.atr";
		FILE* fp = fopen(backup.c_str(), "rb");
		if(fp) {
			ByteStream b(fp);
			fclose(fp);

			fp = fopen(original.c_str(), "wb");
			if(fp) {
				if(fwrite(b.Ptr(), b.GetSize() - 1, 1, fp) != 1) printf("WARNING: Failed to restore backup file %s!\n", backup.c_str());
				fclose(fp);
			} else printf("WARNING: Failed to restore backup file %s!\n", backup.c_str());

		} else {
			printf("WARNING: Failed to restore backup file %s!\n", backup.c_str());
		}
	}
    
    string application::get_actor_var_type(const string &vname) const {
        auto i = m_actor_var_types.find(vname);
        if(i == m_actor_var_types.end()) return "";
        
        return i->second;
    }

    string application::get_actor_block_type(const string &vname) const {
        auto i = m_actor_block_types.find(vname);
        if(i == m_actor_block_types.end()) return "";
        
        return i->second;
    }

	texture* application::getTexture(std::string filename) {
		auto found = m_textures.find(filename);
		if (found == m_textures.end()) {
			texture* t = this->loadTexture(filename);
			m_textures.insert(std::make_pair(filename, t));
			//printf("Loading new texture %s\n", filename.c_str());
			return t;
		}
		else {
			//printf("Texture already found, returning %s\n", filename.c_str());
			return found->second;
		}
	}

	texture* application::loadTexture(std::string filename) {
		i32 w, h, ch;
		unsigned char* Data = SOIL_load_image(filename.c_str(), &w, &h, &ch, 4);
		if (Data)
		{
			texture* t = new texture(w, h, GL_RGBA, GL_UNSIGNED_BYTE, false, Data);
			SOIL_free_image_data(Data);
			return t;
		}
		return 0;
	}
    
    int application::run() {
        glEnable(GL_DEPTH_TEST);
        //glEnable(GL_CULL_FACE);
        glDepthFunc(GL_LESS);
        glCullFace(GL_CW);

        DDRenderInterfaceNull ddRenderIfaceNull;

        dd::initialize(&ddRenderIfaceNull);

        while(m_window->isOpen()) {
			if(m_levelToLoad.length() > 0) {
				load_level(m_levelToLoad);
				m_levelToLoad = "";
				m_selectedActor.actorId = -1;
			}

			if(m_updatingCache) {
				m_cacheProgress = m_fs->get_cache_progress(m_updatingCache, &m_cacheLastFile);
				m_updatingCache = !m_updatingCache;
				if(!m_updatingCache) {
					m_cacheLastFile = "";
					dispatchNamedEvent("actor_caching_completed");
				}
			}

			if(m_restoringBackup) {
				m_restoreProgress = m_fs->get_backup_restore_progress(m_restoringBackup, &m_restoreLastFile);
				m_restoringBackup = !m_restoringBackup;
				if(!m_restoringBackup) {
					m_restoreLastFile = "";
					dispatchNamedEvent("backup_restore_completed");

					if(m_level) load_level(m_level->levelFile()->GetFileName());
				}
			}

            m_window->poll();
            m_window->beginFrame();

			ImGuizmo::BeginFrame();
			ImGuizmo::Enable(true);
			ImGuizmo::SetRect(0, 0, m_framebuffer->w, m_framebuffer->h);
			
			if(m_selectedActor.actorId >= 0) {
				actor* a = m_level->actors()[m_selectedActor.actorId];
				if(a->actorTraits) {
					m_viewChanged = true;
					if(ImGuizmo::IsOver() || ImGuizmo::IsUsing()) {
						m_transformingActor = true;
					} else m_transformingActor = false;
				} else m_transformingActor = false;
			} else m_transformingActor = false;

			if(m_actorToImport) m_viewChanged = true;
            
            if (m_viewChanged)
            {
                m_framebuffer->bind();
                m_framebuffer->clear(vec4(0.5f, 0.5f, 0.5f, 1.0f), 1.0f);
                m_shader->bind();
            
                Frustum f = m_Camera->GetFrustum();

                if(m_level) {
                    vector<actor*> actors = m_level->actors();
                    if (actors.size() > 0) {
                        actors[0]->render(m_shader, m_Camera->GetView(), m_Camera->GetViewProj());
                        for (int i = 1; i < actors.size(); i++) {
                            if (!m_UseCulling || f.intersectFrustumAABB(actors[i]->getBoundingBox()))
                            {
                                actors[i]->render(m_shader, m_Camera->GetView(), m_Camera->GetViewProj());
                            }
                        }
                    }
                }

				if(m_actorToImport && m_cursorOverLevelView) {
					float dpth = m_framebuffer->getDepth(levelViewCursorPos.x, levelViewCursorPos.y, false);

					m_actorImportPos = unProject(vec3(levelViewCursorPos.x, levelViewCursorPos.y, dpth), m_Camera->GetView(), m_Camera->GetViewProj(), vec4(0, 0, levelViewSize.x, levelViewSize.y));
					mat4 model = translate(m_actorImportPos);

					m_actorToImport->render(m_shader, m_Camera->GetView(), m_Camera->GetViewProj(), false, &model);
				}

				dd::flush();
				m_viewChanged = false;
			}
            
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            //render ImGui windows
            for(auto i = m_panels.begin();i != m_panels.end();i++) {
                (*i)->render();
            }
            
            m_window->endFrame();

			m_isImportingActor = m_actorToImport != nullptr;
        }

        dd::shutdown();
        return 0;
    }
}
