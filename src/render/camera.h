#pragma once

#include <render/frustum.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace t4editor {
    class Camera {
        public:
            Camera(float FOV, float AspectRatio, float Near, float Far);
            ~Camera();

            void SetFOV(float FOV);
            void SetAspectRatio(float AspectRatio);
            void SetNear(float Near);
            void SetFar(float Far);
            void SetAngles(glm::vec2 Angles);
            void SetPosition(glm::vec3 Position);

            const Frustum GetFrustum() { return m_Frustum; }
            const glm::vec2 GetAngles() { return m_Angles; }
            const glm::vec3 GetPosition() { return m_Position; }

            const glm::mat4 GetProjection();
            const glm::mat4 GetView();
            const glm::mat4 GetViewProj();

            void ToggleUpdateFrustum() { m_UpdateFrustum = !m_UpdateFrustum; }

            bool m_UpdateFrustum;

        protected:
            void UpdateViewProj() {
                if (m_ViewHasChanged)
                {
                    m_View = glm::eulerAngleXYZ(m_Angles.y, m_Angles.x, 0.0f) * glm::translate(m_Position);
                }

                if (m_ProjHasChanged)
                {
                    m_Proj = glm::perspective(m_FOV, m_AspectRatio, m_Near, m_Far);
                }

                if (m_ProjHasChanged || m_ViewHasChanged)
                {
                    m_ViewProj = m_Proj * m_View;
                    if(m_UpdateFrustum) UpdateFrustum();
                    m_ProjHasChanged = false;
                    m_ViewHasChanged = false;
                }
            }

            void UpdateFrustum();

            bool m_ViewHasChanged, m_ProjHasChanged;
            float m_FOV, m_AspectRatio, m_Near, m_Far;

            glm::vec2 m_Angles;
            glm::vec3 m_Position;
            glm::mat4 m_View;
            glm::mat4 m_Proj;
            glm::mat4 m_ViewProj;

            Frustum m_Frustum;
    };
}