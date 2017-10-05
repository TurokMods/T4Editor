#pragma once
#include <string>
using namespace std;

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
}
