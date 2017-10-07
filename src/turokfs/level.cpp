#include <turokfs/level.h>
#include <turokfs/actor.h>

namespace t4editor {
    level::level(application* app) {
        m_app = app;
        m_atr = nullptr;
    }
    level::~level() {
        for(size_t i = 0;i < m_actors.size();i++) {
            delete m_actors[i];
        }
        if(m_atr) delete m_atr;
    }
    
    bool level::load(const string &file) {
        m_atr = new ATRFile();
        if(!m_atr->Load(file)) {
            delete m_atr;
            return false;
        }
        
        ATIFile* actors = m_atr->GetActors();
        m_actors.push_back(new actor(m_app, m_atr->GetMesh(), nullptr));
        for(size_t i = 0;i < actors->GetActorCount();i++) {
            const ActorDef* def = actors->GetActorDef(i);
            m_actors.push_back(new actor(m_app, def->Actor->GetATR()->GetMesh(), def));
        }
        
        return true;
    }
}
