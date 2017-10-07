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
            unsigned short key;
    };
    
    class load_level_event : public event {
        public:
            load_level_event(const string& _path) : event("load_level") {
                path = _path;
            }
            ~load_level_event() { }
        
            string path;
    };
}
