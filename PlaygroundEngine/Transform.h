#pragma once

#include "Math/math_util.h"

class Transform {
public:
    Transform() {
        m_Position = Vector3(0.0f, 0.0f, 0.0f);
        m_TranslateMatrix = IdentityMatrix;
        m_RotationMatrix = IdentityMatrix;
        m_ScaleMatrix = IdentityMatrix;
    }

    Transform(const Vector3& position, const Matrix4& translateMatrix, const Matrix4& rotationMatrix, const Matrix4& scaleMatrix)
        : m_Position(position), m_TranslateMatrix(translateMatrix), m_RotationMatrix(rotationMatrix), m_ScaleMatrix(scaleMatrix) {
    }

    inline void Translate(const Vector3& translateVector) {
        // World space translation
        m_Position += translateVector;
    }

    inline void Scale(const Vector3& scaleVector) {
        Matrix4 matrix = ScaleMatrix(scaleVector);
        m_ScaleMatrix = matrix * m_ScaleMatrix;
    }

    inline void RotateXAxis(float degreeRadians) {
        Matrix4 matrix = RotateMatrixXAxis(degreeRadians);
        m_RotationMatrix = matrix * m_RotationMatrix;
    }

    inline void RotateYAxis(float degreeRadians) {
        Matrix4 matrix = RotateMatrixYAxis(degreeRadians);
        m_RotationMatrix = matrix * m_RotationMatrix;
    }

    inline void RotateZAxis(float degreeRadians) {
        Matrix4 matrix = RotateMatrixZAxis(degreeRadians);
        m_RotationMatrix = matrix * m_RotationMatrix;
    }

    inline Matrix4 GetTransformMatrix() {
        Matrix4 translateMatrix = TranslateMatrix(m_Position);
        return translateMatrix * m_RotationMatrix * m_ScaleMatrix;
    }

    inline Vector3 GetPosition() {
        return m_Position;
    }

    inline void SetPosition(const Vector3& position) {
        m_Position = position;
    }

private:
    Vector3 m_Position;

    Matrix4 m_TranslateMatrix; // Currently we dont use this
    Matrix4 m_RotationMatrix;
    Matrix4 m_ScaleMatrix;
};

