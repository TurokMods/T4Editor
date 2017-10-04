#include <string>
using namespace std;

namespace t4editor {
    class turokfs;
    class entry {
        public:
            entry(turokfs* fs, const string& name, const string& path);
            virtual ~entry();
        
            string name() const;
            string path() const;
        
            turokfs* fs() const { return m_fs; }
        
        protected:
            string m_name;
            string m_path;
            turokfs* m_fs;
    };
    
    class level_entry : public entry {
        public:
            level_entry(turokfs* fs, const string& name, const string& path);
            virtual ~level_entry();
    };
    
    class actor_entry : public entry {
        public:
            actor_entry(turokfs* fs, const string& name, const string& path);
            virtual ~actor_entry();
    };
}
