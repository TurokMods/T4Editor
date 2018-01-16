#include <render/frustum.h>

namespace t4editor {
    float innerProduct(glm::vec3 first, glm::vec3 second)
    {
        return (first.x * second.x + first.y * second.y + first.z * second.z);
    }
};