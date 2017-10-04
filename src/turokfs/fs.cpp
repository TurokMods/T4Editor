#include <turokfs/fs.h>
#include <turokfs/entry.h>

#include <algorithm>
using namespace std;

namespace t4editor {
    turokfs::turokfs(const string& dir) {
        m_fsys = new FileSystem();
        m_fsys->SetWorkingDirectory(dir);
        
        printf("Filesystem initialized. Parsing game assets...\n");
        
        recursiveParse(".");
        if(m_levels.size() > 0) printf("Found %lu levels\n", m_levels.size());
        if(m_actors.size() > 0) printf("Found %lu actors\n", m_actors.size());
    }
    
    turokfs::~turokfs() {
        delete m_fsys;
    }
    
    void turokfs::recursiveParse(const string &dir) {
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
                    if(ext == "ati") {
                        m_levels.push_back(new level_entry(this, name.substr(0, name.length() - 4), dir + "/" + name));
                    } else if(ext == "atr") {
                        m_actors.push_back(new actor_entry(this, name.substr(0, name.length() - 4), dir + "/" + name));
                    }
                    break;
                }
                case DET_FOLDER: {
                    recursiveParse(dir + "/" + name);
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }
}
