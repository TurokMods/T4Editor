#include <vector>
#include <string>
#include <mutex>
using namespace std;

#include <FileSystem.h>
using namespace Bearclaw;

#include <Turok4.h>
using namespace opent4;

#include <turokfs/sqdb.h>
using namespace sqdb;

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

			void restore_backup();
			float get_backup_restore_progress(bool& is_complete, string* last_file = 0);

            ATRFile* LoadATR(const string& path) { return m_atrStorage->LoadATR(path); }

			ATRStorageInterface* getAtrStorage() const { return m_atrStorage; }
        
        protected:
            void recursiveParseLevels(const string& dir);
            void recursiveParseActors(const string& dir);
            void recursiveParseBackups(const string& dir, vector<string>& files);
			void recursiveParseAll(const string& dir);
            vector<level_entry*> m_levels;
            vector<actor_entry*> m_actors;
            FileSystem* m_fsys;
			Db* m_db;

			float m_Progress;
			float m_lastUsedProgress;
			string m_LastFileProcessed;
			bool m_isProcessing;
			mutex m_Mutex;

			ATRStorageInterface* m_atrStorage;
    };
}
