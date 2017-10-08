#include <string>
#include <vector>
using namespace std;

#include <EngineTypes.h>

#include <window.h>
#include <gui/panel.h>

#include <turokfs/fs.h>
#include <turokfs/entry.h>
#include <turokfs/level.h>
#include <turokfs/actor.h>
#include <render/shader.h>
#include <render/framebuffer.h>
#include <render/texture.h>

#include <glm/glm.hpp>

namespace t4editor {
    class ui_panel;
            
    typedef struct {
        int actorId;
        int actorSubmeshId;
        int actorSubmeshChunkId;
    } actorUnderCursor;
    
    class application : public event_receiver {
        public:
            application(int argc,const char* argv[]);
            ~application();
        
            bool initialize();

            bool load_config();
            void handle_config_var(const string& name, const string& value);
        
            void add_panel(ui_panel* panel);
            void remove_panel(ui_panel* panel);
        
            virtual void onEvent(event* e);
        
            window* getWindow() const { return m_window; }
            string gameDataPath() const { return m_dataPath; }
            string editorDataPath() const { return m_editorDataPath; }
            turokfs* getTurokData() const { return m_fs; }
            framebuffer* getFrame() const { return m_framebuffer; }
        
            level* getLevel() const { return m_level; }
        
            void set_view(const mat4& v, const mat4& p) {
                m_view = v;
                m_proj = p;
                m_vp = p * v;
            }
            mat4 view() const { return m_view; }
            mat4 proj() const { return m_proj; }
            mat4 viewproj() const { return m_vp; }
        
            actorUnderCursor getActorUnderCursor() const { return m_actorUnderCursor; }
            actorUnderCursor getSelectedActor() const { return m_selectedActor; }
            void set_picked_actor_info(actorUnderCursor hovered, actorUnderCursor selected) {
                m_actorUnderCursor = hovered;
                m_selectedActor = selected;
            }

            void load_level(const string& path);
            int run();
        
        

        protected:
            window* m_window;
            turokfs* m_fs;
        
            vector <string> m_args;
            string m_dataPath;
            string m_editorDataPath;
            int m_windowWidth;
            int m_windowHeight;

            actorUnderCursor m_actorUnderCursor;
            actorUnderCursor m_selectedActor;
            
            mat4 m_view;
            mat4 m_proj;
            mat4 m_vp;
            
            vector<ui_panel*> m_panels;
        
            level* m_level;
            shader* m_shader;
            framebuffer* m_framebuffer;
    };
}
