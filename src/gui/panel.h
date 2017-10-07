#pragma once
#include <glm/glm.hpp>
using namespace glm;

#include <string>
#include <vector>
using namespace std;

#include <event.h>

namespace t4editor {
    class application;
    class ui_panel : public event_receiver {
        public:
            ui_panel();
            virtual ~ui_panel();
        
            void add_panel(ui_panel* panel);
            void remove_panel(ui_panel* panel);
        
            // note: toPanel will only be set if the panel is added to another panel, not if it's added to the root through t4editor::application
            virtual void onAttach(ui_panel* toPanel) { }
        
            void setName(const string& name) { m_name = name; }
            string name() const { return m_name; }
            void setSize(const vec2& size) { m_initSize = size; m_sizeUpdated = true; }
            vec2 getSize() const { return m_curSize; }
            void setPosition(const vec2& pos) { m_initPos = pos; m_posUpdated = true; }
            vec2 getPosition() const { return m_curPos; }
            void setOpacity(f32 opacity) { m_bgAlpha = opacity; }
            void setFlagsManually(int flag) { m_manualFlags = flag; }
        
            //flags
            void setCanResize(bool flag) { m_canResize = flag; }
            void setCanMove(bool flag) { m_canMove = flag; }
            void setHasTitleBar(bool flag) { m_hasTitleBar = flag; }
            void setCanClose(bool flag) { m_canClose = flag; }
            void setCanCollapse(bool flag) { m_canCollapse = flag; }
            void setIsWindow(bool flag) { m_isWindow = false; }
        
            void render();
            bool isClosed() const { return !m_isOpen; }
            bool isOpen() const { return m_isOpen; }
            void open() { m_isOpen = true; }
            void close() { m_isOpen = false; }
        
        protected:
            virtual void renderContent() = 0;
            friend class application;
            vector<ui_panel*> m_panels;
            string m_name;
            bool m_isOpen;
            bool m_wasOpen;
        
            vec2 m_initSize;
            vec2 m_curSize;
            bool m_sizeUpdated;
        
            vec2 m_initPos;
            vec2 m_curPos;
            bool m_posUpdated;
        
            f32 m_bgAlpha;
            bool m_canResize;
            bool m_canMove;
            bool m_hasTitleBar;
            bool m_canClose;
            bool m_canCollapse;
            bool m_isWindow;
            int m_manualFlags;
        
            ui_panel* m_parent;
            application* m_app;
    };
}
