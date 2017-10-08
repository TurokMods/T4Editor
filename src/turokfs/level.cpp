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
        m_actors[0]->editor_id = 0;
        for(size_t i = 0;i < actors->GetActorCount();i++) {
            ActorDef* def = actors->GetActorDef(i);
            m_actors.push_back(new actor(m_app, def->Actor->GetATR()->GetMesh(), def));
            m_actors[i]->editor_id = i;
        }
        
        return true;
    }
}
