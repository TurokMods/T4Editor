#ifndef T4_FRUSTUM_H
#define T4_FRUSTUM_H

#include <stdio.h>
#include <cstdio>
#include <glm/glm.hpp>

namespace t4editor {
    struct AABB
    {
        glm::vec3 min, max;
    };

    float innerProduct(glm::vec3 first, glm::vec3 second);

    struct Plane {
        void set(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3)
        {
            normal = glm::normalize(glm::cross(v2 - v1, v3 - v2));
            d = glm::dot(normal, v1);
        }

        glm::vec3 normal, point;
        float d;
    };

    class Frustum {
        public:
            static enum { TOP = 0, BOTTOM, LEFT, RIGHT, NEARP, FARP, NUM_PLANES };
            static enum { OUTSIDE, INTERSECT, INSIDE };
            Plane pl[6];

            Frustum::Frustum() {}
            Frustum::~Frustum() {}

            bool intersectFrustumAABB(const AABB &b)
            {
                // Index box verts
                glm::vec3 box[] = { b.min, b.max };

                for (int i = 0; i < NUM_PLANES; ++i)
                {
                    const Plane& p = pl[i];

                    // p-vertex selection
                    // According to the plane normal we can know the
                    // indices of the positive vertex
                    const int px = static_cast<int>(p.normal.x > 0.0f);
                    const int py = static_cast<int>(p.normal.y > 0.0f);
                    const int pz = static_cast<int>(p.normal.z > 0.0f);

                    // Dot product
                    // project p-vertex on plane normal
                    // (How far is p-vertex from the origin)
                    const float dp =
                        (p.normal.x*box[px].x) +
                        (p.normal.y*box[py].y) +
                        (p.normal.z*box[pz].z);

                    // Doesn't intersect if it is behind the plane
                    if (dp < -p.d) { return false; }
                }
                return true;
            }
    };
};

#endif