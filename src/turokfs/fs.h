#include <vector>
#include <string>
using namespace std;

#include <FileSystem.h>
using namespace Bearclaw;


namespace t4editor {
    class entry;
    class level_entry;
    class actor_entry;
    class turokfs {
        public:
            turokfs(const string& dir);
            ~turokfs();
        
            FileSystem* fileSys() const { return m_fsys; }
        
            vector<level_entry*> levels() const { return m_levels; }
            vector<actor_entry*> actors() const { return m_actors; }
        
        protected:
            void recursiveParseLevels(const string& dir);
            void recursiveParseActors(const string& dir);
            vector<level_entry*> m_levels;
            vector<actor_entry*> m_actors;
            FileSystem* m_fsys;
    };
}
