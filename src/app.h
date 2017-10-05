#include <string>
#include <vector>
using namespace std;

#include <window.h>
#include <gui/panel.h>

#include <turokfs/fs.h>
#include <turokfs/entry.h>

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
            
            vector<ui_panel*> m_panels;
    };
}
