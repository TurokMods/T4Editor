#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace t4editor {
    class window {
        public:
            window(int width, int height);
            ~window();

            bool isOpen() const;
        protected:
            GLFWwindow* m_window;
    };
}
