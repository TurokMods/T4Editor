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
                renderActorProperties();
            }
        
            void renderActorVariables(); //Stored in the ATI file
            void renderActorProperties() { //Stored in the actor's ATR file (global)
                
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
