#include <string>
#include <vector>
#include <unordered_map>
using namespace std;

#include <EngineTypes.h>

#include <window.h>
#include <gui/panel.h>
#include <render/camera.h>

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
        
            void add_panel(ui_panel* panel);
            void remove_panel(ui_panel* panel);
        
            virtual void onEvent(event* e);
        
            window* getWindow() const { return m_window; }
            string gameDataPath() const { return m_dataPath; }
            string editorDataPath() const { return m_editorDataPath; }
            turokfs* getTurokData() const { return m_fs; }
            framebuffer* getFrame() const { return m_framebuffer; }
        
            level* getLevel() const { return m_level; }
            texture* getDefaultTexture() const { return m_defaultTex; }
            
            Camera* GetCamera() const { return m_Camera; }
        
            actorUnderCursor getActorUnderCursor() const { return m_actorUnderCursor; }
            actorUnderCursor getSelectedActor() const { return m_selectedActor; }
            void set_picked_actor_info(actorUnderCursor hovered, actorUnderCursor selected) {
                m_actorUnderCursor = hovered;
                m_selectedActor = selected;
            }

            void load_level(const string& path);

            //threaded processes
            void update_actor_cache();
        
            //actor variables
            void define_actor_var_type(const string& vname, const string& vtype) { m_actor_var_types[vname] = vtype; }
            string get_actor_var_type(const string& vname) const;
        
            //actor properties
            void define_actor_block_type(const string& bname, const string& btype) { m_actor_block_types[bname] = btype; }
            string get_actor_block_type(const string& bname) const;
            int run();
        
            texture* getTexture(std::string file_path);

            float get_cache_update_progress() const { return m_cacheProgress; }
            bool is_updating_cache() const { return m_updatingCache; }
            string get_last_file_cached() const { return m_cacheLastFile; }
            
            void ToggleCulling() { m_UseCulling = !m_UseCulling; }

        protected:
            texture* loadTexture(std::string file_path);
            std::unordered_map<std::string, texture*> m_textures;

            window* m_window;
            turokfs* m_fs;
        
            vector <string> m_args;
            string m_dataPath;
            string m_editorDataPath;
            int m_windowWidth;
            int m_windowHeight;
            int m_windowPosX;
            int m_windowPosY;
            bool m_UseCulling;

            actorUnderCursor m_actorUnderCursor;
            actorUnderCursor m_selectedActor;
            
            Camera* m_Camera;
            
            vector<ui_panel*> m_panels;
        
            level* m_level;
            shader* m_shader;
            texture* m_defaultTex;
            framebuffer* m_framebuffer;

            bool m_updatingCache;
            string m_cacheLastFile;
            float m_cacheProgress;

            unordered_map<string, string> m_actor_var_types;
            unordered_map<string, string> m_actor_block_types;
    };
}
