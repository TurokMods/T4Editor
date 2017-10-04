#include <turokfs/entry.h>
#include <turokfs/fs.h>

namespace t4editor {
    entry::entry(turokfs* fs, const string& name, const string& path) {
        m_name = name;
        m_path = path;
        m_fs = fs;
    }
    entry::~entry() { }
    
    string entry::name() const {
        if(m_name.length() == 0) return "N/A";
        return m_name;
    }
    
    string entry::path() const {
        if(m_path.length() == 0) return "N/A";
        return m_path;
    }
    
    
    level_entry::level_entry(turokfs* fs, const string& name, const string& path) : entry(fs, name, path) {
    }
    
    level_entry::~level_entry() {
    }
    
    actor_entry::actor_entry(turokfs* fs, const string& name, const string& path) : entry(fs, name, path) {
    }
    
    actor_entry::~actor_entry() {
    }
}
