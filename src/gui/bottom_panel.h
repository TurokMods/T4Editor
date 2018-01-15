#pragma once
#include <gui/panel.h>
#include <render/SOIL/SOIL.h>
#include <event.h>

#include <imgui.h>
using namespace ImGui;

#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <util/strnatcmp.hpp>
using namespace std;

namespace t4editor {
	typedef struct actor_cache_entry {
		string path;
		bool renderable;

		//null until loaded
		actor* loadedActor;
		bool failedToLoad;
	};
    class bottom_panel : public ui_panel {
        public:
            bottom_panel() {
                setName("LowerPanel");
                setCanClose(false);
                setFlagsManually(ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing);
				m_lastLogCount = 0;
			}
        
            ~bottom_panel() {
				for(auto i = m_actors.begin();i != m_actors.end();i++) {
					for(size_t a = 0;a < i->second.size();a++) {
						if(i->second[a]->loadedActor) delete i->second[a]->loadedActor;
						delete i->second[a];
					}
				}

				if(m_actorFrame) {
					delete m_actorFrame;
				}
				for(size_t i = 0;i < m_dispTextures.size();i++) {
					delete m_dispTextures[i];
				}
				if(m_shader) delete m_shader;
            }
            
        
            virtual void onAttach(ui_panel* toPanel) {
                vec2 sz = m_app->getWindow()->getSize(false);
                sz.x *= LEVEL_VIEW_WIDTH_FRACTION;
                sz.y -= (sz.y * LEVEL_VIEW_HEIGHT_FRACTION) + 20;
                
                setPosition(vec2(0, m_app->getWindow()->getSize(false).y - sz.y));
                setSize(sz);

				load_actor_cache();
				m_actorFrame = new framebuffer(1, 1, true);

				m_shader = new shader(m_app);
				m_shader->loadFromFile("shaders/preview.vsh", "shaders/preview.fsh");
            }
        
            virtual void onEvent(event* e) {
                if(e->name == "window_resize") {
                    vec2 sz = m_app->getWindow()->getSize();
                    sz.x *= LEVEL_VIEW_WIDTH_FRACTION;
					sz.y -= (sz.y * LEVEL_VIEW_HEIGHT_FRACTION) + 20;
                
					setPosition(vec2(0, m_app->getWindow()->getSize(false).y - sz.y));
                    setSize(sz);
                } else if(e->name == "actor_caching_completed") {
					load_actor_cache();
				}
            }
        
            virtual void renderContent() {
                render_actor_view();
            }

			void load_actor_cache() {
				for(auto i = m_actors.begin();i != m_actors.end();i++) {
					for(size_t a = 0;a < i->second.size();a++) {
						if(i->second[a]->loadedActor) delete i->second[a]->loadedActor;
						delete i->second[a];
					}
				}
				m_actors.clear();
				m_sortedCategories.clear();

				FILE* fp = fopen((m_app->editorDataPath() + "/actor_cache.bin").c_str(), "rb");
				if(!fp) {
					m_app->dispatchNamedEvent("update_actor_cache");
					return;
				}

				ByteStream* data = new ByteStream(fp);
				fclose(fp);
				if(data->GetSize() == 0) {
					m_app->dispatchNamedEvent("update_actor_cache");
					delete data;
					return;
				}

				while(!data->AtEnd(1)) {
					string category = data->GetString();
					unsigned int count = data->GetInt32();
					m_actors[category] = vector<actor_cache_entry*>();
					m_sortedCategories.push_back(category);
					auto it = m_actors.find(category);
					for(unsigned int i = 0;i < count;i++) {
						actor_cache_entry* entry = new actor_cache_entry();
						entry->path = data->GetString();
						entry->renderable = (bool)data->GetByte();
						entry->loadedActor = nullptr;
						entry->failedToLoad = false;

						it->second.push_back(entry);
					}
				}
				delete data;

				sort(m_sortedCategories.begin(), m_sortedCategories.end(), compareNat);
			}

			void prepare_actors() {
				if(!m_actors.empty()) {
					int num_loaded = 0;
					auto category = m_actors.find(m_currentCategory);
					if(category != m_actors.end()) {
						for(int a = 0;a < category->second.size();a++) {
							actor_cache_entry* entry = category->second[a];
							if(!entry->renderable) continue;
							if(!entry->loadedActor && num_loaded < 2) {
								//the turokfs class will take care of deallocating it, having it in memory will speed up the load time of other levels too
								ATRFile* file = m_app->getTurokData()->LoadATR(entry->path);
								if(file) {
									entry->loadedActor = new actor(m_app, file->GetMesh(), nullptr);
								}
							}
						}
					}
				}
			}

			void render_actor_view() {
				prepare_actors();

				//headers
				BeginChild("##categories_hdr", ImVec2(320, 20), true, ImGuiWindowFlags_NoMove);
					Text("Categories");
				EndChild();

				SameLine(321.0f);
				
				BeginChild(("##" + m_currentCategory + "_hdr").c_str(), ImVec2(0, 20), true, ImGuiWindowFlags_NoMove);
					Text("Actors");
				EndChild();

				//views
				BeginChild("##categories", ImVec2(320, 0), true);
					if(!m_actors.empty()) {
						for(size_t i = 0;i < m_sortedCategories.size();i++) {
							auto c = m_actors.find(m_sortedCategories[i]);
							if(c != m_actors.end()) {
								int count = c->second.size();
								ostringstream title;
								title << (c->first.length() == 0 ? "[no name]" : c->first) << " (" << count << ")";
								bool selected = m_currentCategory == c->first;
								if(Selectable(title.str().c_str(), selected)) {
									m_currentCategory = c->first;
								}
							}
						}
					} else {
						Text("Loading categories...");
					}
				EndChild();

				SameLine(321.0f);
				
				BeginChild(("##" + m_currentCategory + "ap_view_child").c_str(), ImVec2(0,0), true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysHorizontalScrollbar);
					if(!m_actors.empty()) {
						ImVec2 img_size = ImVec2(GetWindowHeight() - 15, GetWindowHeight() - 15);
						m_actorFrame->w = img_size.x;
						m_actorFrame->h = img_size.y;
						auto category = m_actors.find(m_currentCategory);
						if(category != m_actors.end()) {
							m_shader->bind();
							ImVec2 cp = GetCursorPos();
							for(int a = 0;a < category->second.size();a++) {
								actor_cache_entry* entry = category->second[a];
								if(entry->loadedActor) {

									//Make sure there's always enough textures for the displayed row
									//(really this is way better than one texture per actor)
									if(a >= m_dispTextures.size()) {
										while(a >= m_dispTextures.size()) m_dispTextures.push_back(new texture(1, 1, GL_RGBA, GL_UNSIGNED_BYTE, false));
									}

									texture* tex = m_dispTextures[a];
									if(m_actorFrame->attachments.size() == 1) m_actorFrame->attachments[0] = tex;
									else m_actorFrame->attachments.push_back(tex);

									m_actorFrame->bind();
									m_actorFrame->clear(vec4(0.5f, 0.5f, 0.5f, 0.2f),1.0f);

									float camDist = length(entry->loadedActor->getMaxBound() - entry->loadedActor->getMinBound()) * (0.7f + (sinf(radians(float(glfwGetTime() * 4.0f))) * 0.25f));
									vec3 center = (entry->loadedActor->getMaxBound() + entry->loadedActor->getMinBound()) * 0.5f;
									mat4 view = lookAt(vec3(1, 0.5f + (sinf(radians(float(glfwGetTime() * 3.0f))) * 0.5f), 1) * camDist, center, vec3(0, 1, 0)) * glm::rotate(radians(float(glfwGetTime() * 8.0f)), vec3(0, 1, 0));
									mat4 proj = perspective(radians(65.0f), img_size.x / img_size.y, 0.0001f, 200.0f);

									entry->loadedActor->render(m_shader, view, proj * view, true);

									Image((GLuint*)tex->id, img_size, ImVec2(0, 1), ImVec2(1, 0));
								} else {
									if(entry->renderable) {
										if(entry->failedToLoad) {
											SetCursorPos(ImVec2(((img_size.x + 5) * a) + (img_size.x / 2) - 30.0f, (img_size.y / 2) - 10.0f));
											Text("Actor failed to load");
										}
										else {
											SetCursorPos(ImVec2(((img_size.x + 5) * a) + (img_size.x / 2) - 20.0f, (img_size.y / 2) - 10.0f));
											Text("Loading...");
										}
									}
								}
								
								SetCursorPos(ImVec2(((img_size.x + 5) * a) + 5.0f,cp.y + 2));
								Text("%s", entry->path.substr(entry->path.find_last_of('/')).c_str());
								SetCursorPos(ImVec2((img_size.x + 5) * (a + 1),cp.y));
							}
							
							glBindFramebuffer(GL_FRAMEBUFFER, 0);
						} else {
							Text("No actors in category");
						}
					} else {
						Text("Loading actors...");
					}
				EndChild();
			}
		
		protected:
			framebuffer* m_actorFrame;
			shader* m_shader;
			string m_currentCategory;
			size_t m_lastLogCount;
			unordered_map<string, vector<actor_cache_entry*> > m_actors;
			vector<string> m_sortedCategories;
			vector<texture*> m_dispTextures;
    };
}
