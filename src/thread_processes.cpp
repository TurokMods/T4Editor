#include <app.h>
#include <thread>

namespace t4editor {
	void update_actor_func(application* app, const string& path) {
		app->getTurokData()->update_actor_cache(path);
	}

	void application::update_actor_cache() {
		string path = m_editorDataPath + "/actor_cache.bin";
		thread t = thread(update_actor_func, this, path);
		t.detach();
	}
}
