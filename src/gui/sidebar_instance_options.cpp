#include <app.h>
#include <gui/ui.h>
#include <gui/main_window.h>
#include <gui/sidebar.h>
#include <imgui_internal.h>

namespace t4editor {
    void sidebar::renderInstanceOptions() {
		if(CollapsingHeader("Options")) {
			Indent(10.0f);
				if(Actor) {
					if(Button("Duplicate Actor", ImVec2(GetWindowContentRegionWidth() - 15.0f, 20.0f))) {
						Level->levelFile()->GetActors()->DuplicateActor(Actor->actorTraits);
						m_app->dispatchNamedEvent("actor_added");
					}
				} else if(Level) {
					Text("No options to show yet");
				}
				else {
					Text("No actor selected");
				}
			Unindent(10.0f);
		}
    }
};
