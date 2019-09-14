#include "PGCamera.h"


PGCamera::PGCamera() {

}

void PGCamera::SetFrustum(uint32_t imageWidth, uint32_t imageHeight, float nearPlaneDistance, float farPlaneDistance, float fovRadians) {
    m_ProjectionMatrix = PerspectiveMatrix(imageWidth, imageHeight, nearPlaneDistance, farPlaneDistance, fovRadians);
}

void PGCamera::SetView(Vector3 cameraPosition, Vector3 targetPoint) {
    // Left-handed look-at function
    const Vector3 upVector(0.0f, 1.0f, 0.0f);
    const Vector3 cameraZ = Normalize(targetPoint - cameraPosition);
    const Vector3 cameraX = Normalize(CrossProduct(upVector, cameraZ));
    const Vector3 cameraY = Normalize(CrossProduct(cameraZ, cameraX));
    Matrix4 viewMatrix;
    viewMatrix[0] = Vector4(cameraX, -DotProduct(cameraPosition, cameraX));
    viewMatrix[1] = Vector4(cameraY, -DotProduct(cameraPosition, cameraY));
    viewMatrix[2] = Vector4(cameraZ, -DotProduct(cameraPosition, cameraZ));
    viewMatrix[3][3] = 1.0f;

    m_ViewMatrix = viewMatrix;
    m_Position = cameraPosition;
}

