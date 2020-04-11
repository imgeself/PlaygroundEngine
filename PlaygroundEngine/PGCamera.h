#pragma once

#include "Core.h"
#include "Transform.h"
#include "Math/math_util.h"

class PG_API PGCamera {
public:
    PGCamera();
    ~PGCamera() = default;

    void SetFrustum(uint32_t imageWidth, uint32_t imageHeight, float nearPlaneDistance, float farPlaneDistance, float fovRadians);
    void SetView(Vector3 cameraPosition, Vector3 targetPoint);

    void UpdateAspectRatio(uint32_t width, uint32_t height);
    void TransformCamera(Transform* transform);

    inline Vector3 GetPosition() {
        return m_Position;
    }

    inline Matrix4 GetProjectionMatrix() {
        return m_ProjectionMatrix;
    }

    inline Matrix4 GetViewMatrix() {
        return m_ViewMatrix;
    }

    inline Matrix4 GetProjectionViewMatrix() {
        return m_ProjectionMatrix * m_ViewMatrix;
    }

    inline Matrix4 GetInverseProjectionViewMatrix() {
        Matrix4 projectionViewMatrix = GetProjectionViewMatrix();
        return Inverse(projectionViewMatrix);
    }
private:
    Matrix4 m_ProjectionMatrix;
    Matrix4 m_ViewMatrix;

    Vector3 m_Position;

    float m_NearPlaneDistance = 0.1f;
    float m_FarPlaneDistance = 100.0f;
    float m_FovRadians = (PI / 4.0f);
};

