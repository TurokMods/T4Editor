#include <turokfs/level.h>
#include <turokfs/actor.h>
#include <app.h>
#include <EngineTypes.h>


namespace t4editor {
    level::level(application* app) {
        m_app = app;
        m_atr = nullptr;
    }
    
	level::~level() {
        for(size_t i = 0;i < m_actors.size();i++) {
            delete m_actors[i];
        }
    }

    bool level::load(const string &file) {
        m_atr = m_app->getTurokData()->getAtrStorage()->LoadATR(file);
        if(!m_atr) return false;
        
        m_actors.push_back(new actor(m_app, m_atr->GetMesh(), nullptr, m_atr));
        m_actors[0]->editor_id = 0;

        ATIFile* actors = m_atr->GetActors();
        for(size_t i = 0;i < actors->GetActorCount();i++) {
            ActorDef* def = actors->GetActorDef(i);
            m_actors.push_back(new actor(m_app, def->Actor->GetATR()->GetMesh(), def, def->Actor->GetATR()));
            m_actors[i + 1]->editor_id = i + 1;
        }
        
        return true;
    }
	void level::actor_added() {
		ActorDef* def = m_atr->GetActors()->GetActorDef(m_atr->GetActors()->GetActorCount() - 1);
		m_actors.push_back(new actor(m_app, def->Actor->GetATR()->GetMesh(), def, def->Actor->GetATR()));
		m_actors[m_actors.size() - 1]->editor_id = m_actors.size() - 1;
	}
}
