#include <string>
#include <vector>
using namespace std;

namespace t4editor {
    class application {
        public:
            application(int argc,const char* argv[]);
            ~application();

            bool load_config();
            void handle_config_var(const string& name, const string& value);

            int run();

        protected:
            vector <string> m_args;
            string m_dataPath;
    };
}
