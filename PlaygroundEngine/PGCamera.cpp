#include "PGCamera.h"


PGCamera::PGCamera() {

}

void PGCamera::SetFrustum(uint32_t imageWidth, uint32_t imageHeight, float nearPlaneDistance, float farPlaneDistance, float fovRadians) {
    m_ProjectionMatrix = PerspectiveMatrix(imageWidth, imageHeight, nearPlaneDistance, farPlaneDistance, fovRadians);
}

void PGCamera::SetView(Vector3 cameraPosition, Vector3 targetPoint) {
    // Left-handed look-at function
    m_ViewMatrix = LookAtLH(cameraPosition, targetPoint);
    m_Position = cameraPosition;
}

