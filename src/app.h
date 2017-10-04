#include <string>
#include <vector>
using namespace std;

#include <window.h>

namespace t4editor {
    class application {
        public:
            application(int argc,const char* argv[]);
            ~application();

            bool load_config();
            void handle_config_var(const string& name, const string& value);

            int run();

        protected:
            window* m_window;
            vector <string> m_args;
            string m_dataPath;
            int m_windowWidth;
            int m_windowHeight;
    };
}
