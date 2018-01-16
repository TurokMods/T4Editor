#ifndef T4_FRUSTUM_H
#define T4_FRUSTUM_H

#include <stdio.h>
#include <cstdio>
#include <glm/glm.hpp>

namespace t4editor {
    struct AABB
    {
        glm::vec3 min, max;

        void transform(glm::mat4 trans, glm::vec3 mn, glm::vec3 mx)
        {
            glm::vec4 verts[8];
            verts[0] = glm::vec4(mn, 1);
            verts[1] = glm::vec4(mx, 1);
            verts[2] = glm::vec4(mn.x, mn.y, mx.z, 1);
            verts[3] = glm::vec4(mn.x, mx.y, mn.z, 1);
            verts[4] = glm::vec4(mx.x, mn.y, mn.z, 1);
            verts[5] = glm::vec4(mn.x, mx.y, mn.z, 1);
            verts[6] = glm::vec4(mn.x, mx.y, mx.z, 1);
            verts[7] = glm::vec4(mx.x, mx.y, mn.z, 1);

            min = max = glm::vec3(trans * verts[0]);
            for (int i = 1; i < 8; i++) {
                glm::vec4 vert = trans * verts[i];
                if (vert.x > max.x) max.x = vert.x;
                if (vert.y > max.y) max.y = vert.y;
                if (vert.z > max.z) max.z = vert.z;

                if (vert.x < min.x) min.x = vert.x;
                if (vert.y < min.y) min.y = vert.y;
                if (vert.z < min.z) min.z = vert.z;
            }
        }
    };

    struct Plane {
        void set(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3)
        {
            normal = glm::normalize(glm::cross(v2 - v1, v3 - v2));
            d = glm::dot(normal, v1);
        }

        glm::vec3 normal;
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