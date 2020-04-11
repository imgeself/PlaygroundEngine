#include "PGCamera.h"


PGCamera::PGCamera()
    : m_Position(0.0f, 3.0f, -10.0f)
{

}

void PGCamera::SetFrustum(uint32_t imageWidth, uint32_t imageHeight, float nearPlaneDistance, float farPlaneDistance, float fovRadians) {
    m_ProjectionMatrix = PerspectiveMatrixLH(imageWidth, imageHeight, nearPlaneDistance, farPlaneDistance, fovRadians);

    m_NearPlaneDistance = nearPlaneDistance;
    m_FarPlaneDistance = farPlaneDistance;
    m_FovRadians = fovRadians;
}

void PGCamera::SetView(Vector3 cameraPosition, Vector3 targetPoint) {
    // Left-handed look-at function
    m_ViewMatrix = LookAtLH(cameraPosition, targetPoint, Vector3(0.0f, 1.0f, 0.0f));
    m_Position = cameraPosition;
}

void PGCamera::UpdateAspectRatio(uint32_t width, uint32_t height) {
    m_ProjectionMatrix = PerspectiveMatrixLH(width, height, m_NearPlaneDistance, m_FarPlaneDistance, m_FovRadians);
}

void PGCamera::TransformCamera(Transform* transform) {
    m_Position = transform->position;

	Matrix4 orientation = transform->rotationMatrix;
    Vector3 up = (orientation * Vector4(0.0f, 1.0f, 0.0f, 0.0f)).xyz();
    Vector3 target = (orientation * Vector4(0.0f, 0.0f, 1.0f, 0.0f)).xyz();

    Vector3 lookAtTarget = m_Position + target;
	m_ViewMatrix = LookAtLH(m_Position, lookAtTarget, up);
}


