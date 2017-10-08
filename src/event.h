#pragma once
#include <string>
using namespace std;

#include <glm/glm.hpp>
using namespace glm;

struct GLFWwindow;

namespace t4editor {
    class event {
        public:
            event(const string& _name) : name(_name) { }
            virtual ~event() { }
        
            string name;
    };
    
    class event_receiver {
        public:
            event_receiver() { }
            virtual ~event_receiver() { }
        
            void dispatchNamedEvent(const string& simpleEventName) {
                event e(simpleEventName);
                onEvent(&e);
            }
            virtual void onEvent(event* e) = 0;
    };
    
    class app_resize_event : public event {
        public:
            app_resize_event(int w, int h, GLFWwindow* _window) : event("window_resize") {
                new_width = w;
                new_height = h;
                window = _window;
            }
            ~app_resize_event() { }
            
            int new_width;
            int new_height;
            GLFWwindow* window;
    };
    
    class input_event : public event {
        public:
            enum event_type {
                ET_MOUSE_LEFT_DOWN,
                ET_MOUSE_LEFT_UP,
                ET_MOUSE_MIDDLE_DOWN,
                ET_MOUSE_MIDDLE_UP,
                ET_MOUSE_RIGHT_DOWN,
                ET_MOUSE_RIGHT_UP,
                ET_KEY_DOWN,
                ET_KEY_UP,
                ET_MOUSE_MOVE,
                ET_SCROLL,
            };
            input_event() : event("input_event") {
            }
            ~input_event() {
            }
        
            double time;
            event_type type;
            vec2 cursorPosition;
            double scrollDelta;
            unsigned int key;
    };
    
    class load_level_event : public event {
        public:
            load_level_event(const string& _path) : event("load_level") {
                path = _path;
            }
            ~load_level_event() { }
        
            string path;
    };
    
    class level;
    class actor;
    class actor_mesh;
    class actor_selection_event : public event {
        public:
            actor_selection_event(level* l, actor* a, actor_mesh* m, int aid, int asmid, int asmcid, bool wasDeselected) : event("actor_selection") {
                parentLevel = l;
                selectedActor = a;
                actorMesh = m;
                actorId = aid;
                subMeshId = asmid;
                meshChunkId = asmcid;
                deselected = wasDeselected;
            }
            ~actor_selection_event() {
            }
        
            level* parentLevel;
            actor* selectedActor;
            actor_mesh* actorMesh;
            int actorId;
            int subMeshId;
            int meshChunkId;
            bool deselected;
    };
}
