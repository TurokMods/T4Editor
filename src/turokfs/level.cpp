#include <turokfs/level.h>
#include <turokfs/actor.h>

#include <EngineTypes.h>

#include <render/SOIL/SOIL.h>

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

	texture* level::loadTexture(std::string filename) {
		i32 w, h, ch;
		unsigned char* Data = SOIL_load_image(filename.c_str(), &w, &h, &ch, 4);
		if (Data)
		{
			texture* t = new texture(w, h, GL_RGBA, GL_UNSIGNED_BYTE, false, Data);
			SOIL_free_image_data(Data);
			return t;
		}
		return 0;
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
