#include <string>
#include <vector>
using namespace std;

#include <window.h>
#include <gui/panel.h>

#include <turokfs/fs.h>
#include <turokfs/entry.h>
#include <turokfs/level.h>
#include <turokfs/actor.h>
#include <render/shader.h>
#include <render/framebuffer.h>
#include <render/texture.h>

#include <OpenGL/gl.h>

namespace t4editor {
    class ui_panel;
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
        
            mat4 view() const { return m_view; }
            mat4 proj() const { return m_proj; }
            mat4 viewproj() const { return m_vp; }

            void load_level(const string& path);
            int run();
        
            int actorUnderCursor;
            int actorSubmeshUnderCursor;
            int actorSubmeshChunkUnderCursor;

        protected:
            window* m_window;
            turokfs* m_fs;
        
            vector <string> m_args;
            string m_dataPath;
            string m_editorDataPath;
            int m_windowWidth;
            int m_windowHeight;
        
            int m_inputEnabledCounter;
            vec2 m_curCursor;
            vec2 m_camAngles;
            vec3 m_camPos;
            float m_fov;
            bool m_mouseBtnDown[3];
            bool m_keyDown[256];
            mat4 m_view;
            mat4 m_proj;
            mat4 m_vp;
            
            vector<ui_panel*> m_panels;
        
            level* m_level;
            shader* m_shader;
            framebuffer* m_framebuffer;
    };
}
