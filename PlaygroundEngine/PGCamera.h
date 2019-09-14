#pragma once

#include "Core.h"
#include "Math/math_util.h"

class PG_API PGCamera {
public:
    PGCamera();
    ~PGCamera() = default;

    void SetFrustum(uint32_t imageWidth, uint32_t imageHeight, float nearPlaneDistance, float farPlaneDistance, float fovRadians);
    void SetView(Vector3 cameraPosition, Vector3 targetPoint);

    Vector3 GetPosition() {
        return m_Position;
    }
    Matrix4 GetProjectionMatrix() {
        return m_ProjectionMatrix;
    }
    Matrix4 GetViewMatrix() {
        return m_ViewMatrix;
    }
    Matrix4 GetProjectionViewMatrix() {
        return m_ProjectionMatrix * m_ViewMatrix;
    }
private:
    Matrix4 m_ProjectionMatrix;
    Matrix4 m_ViewMatrix;

    Vector3 m_Position;
};

