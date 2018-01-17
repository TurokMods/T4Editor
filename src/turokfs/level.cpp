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
        m_atr = m_app->getTurokData()->getAtrStorage()->LoadATR(file, true);
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
	actor* level::actor_added() {
		ActorDef* def = m_atr->GetActors()->GetActorDef(m_atr->GetActors()->GetActorCount() - 1);
		m_actors.push_back(new actor(m_app, def->Actor->GetATR()->GetMesh(), def, def->Actor->GetATR()));
		m_actors[m_actors.size() - 1]->editor_id = m_actors.size() - 1;

		actorUnderCursor hovered = m_app->getActorUnderCursor();
		actorUnderCursor selected;
		selected.actorId = m_actors.size() - 1;
		selected.actorSubmeshId = -1;
		selected.actorSubmeshChunkId = -1;
		m_app->set_picked_actor_info(hovered, selected);

		return m_actors[m_actors.size() - 1];
	}
	void level::actor_deleted(actor_deleted_event* e) {
		m_actors.erase(m_actors.begin() + e->actorToDelete->editor_id);

		actorUnderCursor nullSelection;
		nullSelection.actorId = -1;
		nullSelection.actorSubmeshId = -1;
		nullSelection.actorSubmeshChunkId = -1;
		m_app->set_picked_actor_info(nullSelection, nullSelection);

		for(size_t i = e->actorToDelete->editor_id;i < m_actors.size();i++) {
			m_actors[i]->editor_id = i;
		}
		
		//don't delete the actor, the actor panel is using it I guess
		//delete e->actorToDelete;
	}
}
