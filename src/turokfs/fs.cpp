#include <turokfs/fs.h>
#include <turokfs/entry.h>
#include <unordered_map>
#include <Turok4.h>
#include <logger.h>
using namespace opent4;

#include <algorithm>
#include <sstream>
using namespace std;

namespace t4editor {
    turokfs::turokfs(const string& dir) {
        m_fsys = new FileSystem();
        m_fsys->SetWorkingDirectory(dir);
        
        printf("Filesystem initialized. Parsing game assets...\n");
        
        recursiveParseLevels("data/levels");
        if(m_levels.size() > 0) printf("Found %lu levels\n", m_levels.size());
        recursiveParseActors("data/actors");
        if(m_actors.size() > 0) printf("Found %lu actors\n", m_actors.size());
    }
    
    turokfs::~turokfs() {
        delete m_fsys;
    }

	void turokfs::update_actor_cache(const string& file) {
		m_cacheMutex.lock();
		m_cacheProgress = 0.0f;
		m_lastUsedCacheProgress = 0.0f;
		m_isUpdatingCache = true;
		m_cacheMutex.unlock();

		float load_weight = 0.95f;
		float save_weight = 0.04f;

		int actor_count = 0;
		int actor_page = 1;

		unordered_map <string, vector<pair<bool, string> > > category_actors;
		for(size_t i = 0;i < m_actors.size();i++) {
			ATRFile* atr = LoadATR(m_actors[i]->path());
			if(!atr) continue;

			string category = atr->GetActorCode();
			//There is over 1000 in the "Actor" category. It needs to be broken up, this should be enough
			if(category == "Actor") {
				ostringstream o;
				o << " [page: " << actor_page << "]";
				category += o.str();
				actor_count++;
				if(actor_count % 70 == 0) actor_page++;
			}

			if(category_actors.find(category) == category_actors.end()) category_actors[category] = vector<pair<bool, string> >();
			category_actors[category].push_back(pair<bool, string>(atr->GetMesh() != nullptr, m_actors[i]->path()));

			m_cacheMutex.lock();
			m_cacheProgress = (float(i) / float(m_actors.size())) * load_weight;
			m_cacheLastFileProcessed = atr->GetFileName();
			m_cacheLastFileProcessed.substr(m_cacheLastFileProcessed.find_last_of('/'));
			m_cacheMutex.unlock();
		}

		ByteStream* output = new ByteStream();
		int aid = 0;
		for(auto i = category_actors.begin();i != category_actors.end();i++) {
			output->WriteString(i->first); //category (actor code)
			output->WriteInt32(i->second.size()); //number of actors in category
			vector<pair<bool, string> > actors = i->second;
			for(auto p = actors.begin();p != actors.end();p++) {
				output->WriteString(p->second); //path to file (turok's fake relative path from Y:\)
				output->WriteByte(p->first); //is_renderable

				m_cacheMutex.lock();
				m_cacheProgress = load_weight + ((float(aid) / float(m_actors.size())) * save_weight);
				m_cacheMutex.unlock();
				m_cacheLastFileProcessed = p->second;
				aid++;
			}
		}
				
		FILE* fp = fopen(file.c_str(), "wb");
		output->SetOffset(0);
		fwrite(output->Ptr(), output->GetSize(), 1, fp);
		fclose(fp);
		delete output;

		m_cacheMutex.lock();
		m_cacheProgress = 1.0f;
		m_isUpdatingCache = false;
		m_cacheMutex.unlock();
	}
	float turokfs::get_cache_progress(bool& is_complete, string* last_file) {
		if(m_cacheMutex.try_lock()) {
			m_lastUsedCacheProgress = m_cacheProgress;
			if(last_file) *last_file = m_cacheLastFileProcessed;
			is_complete = !m_isUpdatingCache;
			m_cacheMutex.unlock();
		} else is_complete = false;

		return m_lastUsedCacheProgress;
	}
	ATRFile* turokfs::LoadATR(const string& path) {
		ATRFile* file = new ATRFile();
		if(!file->Load(path)) {
			printf("Failed to load an ATR file referenced by the level\n");
			printf("The file was %s\n", path.c_str());
			delete file;
			return 0;
		}
		return file;
	}
    
    void turokfs::recursiveParseLevels(const string &dir) {
        DirectoryInfo* d = m_fsys->ParseDirectory(dir);
        if(!d) {
            printf("Error reading directory '%s'\n", dir.c_str());
            return;
        }
        
        for(u32 i = 0;i < d->GetEntryCount();i++) {
            string name = d->GetEntryName(i);
            if(name == "." || name == "..") continue;
            
            switch(d->GetEntryType(i)) {
                case DET_FILE: {
                    string ext = d->GetEntryExtension(i);
                    transform(ext.begin(), ext.end(), ext.begin(), tolower);
                    if(ext == "atr") {
                        m_levels.push_back(new level_entry(this, name.substr(0, name.length() - 4), dir + "/" + name));
                    }
                    break;
                }
                case DET_FOLDER: {
                    recursiveParseLevels(dir + "/" + name);
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }
    void turokfs::recursiveParseActors(const string &dir) {
        DirectoryInfo* d = m_fsys->ParseDirectory(dir);
        if(!d) {
            printf("Error reading directory '%s'\n", dir.c_str());
            return;
        }
        
        for(u32 i = 0;i < d->GetEntryCount();i++) {
            string name = d->GetEntryName(i);
            if(name == "." || name == "..") continue;
            
            switch(d->GetEntryType(i)) {
                case DET_FILE: {
                    string ext = d->GetEntryExtension(i);
                    transform(ext.begin(), ext.end(), ext.begin(), tolower);
                    if(ext == "atr") {
                        m_actors.push_back(new actor_entry(this, name.substr(0, name.length() - 4), dir + "/" + name));
                    }
                    break;
                }
                case DET_FOLDER: {
                    recursiveParseActors(dir + "/" + name);
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }
}
