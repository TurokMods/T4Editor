#include <render/camera.h>

namespace t4editor {
    Camera::Camera(float FOV, float AspectRatio, float Near, float Far)
    {
        m_ViewHasChanged = true;
        m_ProjHasChanged = true;
        m_UpdateFrustum = true;

        m_FOV = FOV;
        m_AspectRatio = AspectRatio;
        m_Near = Near;
        m_Far = Far;

        m_Angles = glm::vec2(0.0f, 0.0f);
        m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
        UpdateViewProj();
    }

    Camera::~Camera() {}

    void Camera::SetFOV(float FOV)
    {
        m_ProjHasChanged = true;
        m_FOV = FOV;
    }

    void Camera::SetAspectRatio(float AspectRatio)
    {
        m_ProjHasChanged = true;
        m_AspectRatio = AspectRatio;
    }

    void Camera::SetNear(float Near)
    {
        m_ProjHasChanged = true;
        m_Near = Near;
    }

    void Camera::SetFar(float Far)
    {
        m_ProjHasChanged = true;
        m_Far = Far;
    }

    void Camera::SetAngles(glm::vec2 Angles)
    {
        m_ViewHasChanged = true;
        m_Angles = Angles;
    }

    void Camera::SetPosition(glm::vec3 Position)
    {
        m_ViewHasChanged = true;
        m_Position = Position;
    }

    void Camera::UpdateFrustum() {
        float Hnear = 2 * tan(m_FOV * 0.5) * m_Near;
        float Wnear = Hnear * m_AspectRatio;

        float Hfar = 2 * tan(m_FOV / 2) * m_Far;
        float Wfar = Hfar * m_AspectRatio;

        glm::vec3 p = m_Position;
        glm::mat4 CamTransform = glm::eulerAngleXYZ(m_Angles.y, m_Angles.x, 0.0f);
        glm::vec3 d = glm::normalize(glm::vec3(glm::vec4(0, 0, 1, 1) * CamTransform));
        glm::vec3 up = glm::normalize(glm::vec3(glm::vec4(0, 1, 0, 1) * CamTransform));
        glm::vec3 right = glm::normalize(glm::vec3(glm::vec4(1, 0, 0, 1) * CamTransform));
        printf("Camera direction %f, %f, %f\n", d.x, d.y, d.z);

        glm::vec3 fc = p + d * m_Far;
        glm::vec3 nc = p + d * m_Near;

        glm::vec3 ftl = fc + (up * Hfar / 2.0f) - (right * Wfar / 2.0f);
        glm::vec3 ftr = fc + (up * Hfar / 2.0f) + (right * Wfar / 2.0f);
        glm::vec3 fbl = fc - (up * Hfar / 2.0f) - (right * Wfar / 2.0f);
        glm::vec3 fbr = fc - (up * Hfar / 2.0f) + (right * Wfar / 2.0f);

        glm::vec3 ntl = nc + (up * Hnear / 2.0f) - (right * Wnear / 2.0f);
        glm::vec3 ntr = nc + (up * Hnear / 2.0f) + (right * Wnear / 2.0f);
        glm::vec3 nbl = nc - (up * Hnear / 2.0f) - (right * Wnear / 2.0f);
        glm::vec3 nbr = nc - (up * Hnear / 2.0f) + (right * Wnear / 2.0f);

        m_Frustum.pl[Frustum::TOP].set(ntr, ntl, ftl);
        m_Frustum.pl[Frustum::BOTTOM].set(nbl, nbr, fbr);
        m_Frustum.pl[Frustum::LEFT].set(ntl, nbl, fbl);
        m_Frustum.pl[Frustum::RIGHT].set(nbr, ntr, fbr);
        m_Frustum.pl[Frustum::NEARP].set(ntl, ntr, nbr);
        m_Frustum.pl[Frustum::FARP].set(ftr, ftl, fbl);

    }

    const glm::mat4 Camera::GetProjection() {
        this->UpdateViewProj();
        return m_Proj;
    }

    const glm::mat4 Camera::GetView() {
        this->UpdateViewProj();
        return m_View;
    }

    const glm::mat4 Camera::GetViewProj() {
        this->UpdateViewProj();
        return m_ViewProj;
    }
}