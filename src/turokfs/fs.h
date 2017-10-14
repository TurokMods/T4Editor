#include <vector>
#include <string>
#include <mutex>
using namespace std;

#include <FileSystem.h>
using namespace Bearclaw;

namespace opent4 {
	class ATRFile;
}
using namespace opent4;

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

			void update_actor_cache(const string& file);
			float get_cache_progress(bool& is_complete, string* last_file = 0);
            ATRFile* LoadATR(const string& path);
        
        protected:
            void recursiveParseLevels(const string& dir);
            void recursiveParseActors(const string& dir);
            vector<level_entry*> m_levels;
            vector<actor_entry*> m_actors;
            FileSystem* m_fsys;

			float m_cacheProgress;
			float m_lastUsedCacheProgress;
			string m_cacheLastFileProcessed;
			bool m_isUpdatingCache;
			mutex m_cacheMutex;
    };
}
