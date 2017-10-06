#include <turokfs/level.h>
#include <turokfs/actor.h>

namespace t4editor {
    level::level(application* app) {
        m_app = app;
        m_atr = nullptr;
    }
    level::~level() {
    }
    
    bool level::load(const string &file) {
        m_atr = new ATRFile();
        if(!m_atr->Load(file)) {
            delete m_atr;
            return false;
        }
        
        ATIFile* actors = m_atr->GetActors();
        m_actors.push_back(new actor(m_app, m_atr->GetActor()));
        for(size_t i = 0;i < actors->GetActorCount();i++) {
            const Actor* actor_def = actors->GetActorDef(i)->Actor->GetActor();
            m_actors.push_back(new actor(m_app, actor_def));
        }
        
        return true;
    }
}
