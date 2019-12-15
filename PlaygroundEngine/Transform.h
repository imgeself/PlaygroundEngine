#pragma once

#include "Math/math_util.h"

class Transform {
public:
    Transform() {
        position = Vector3(0.0f, 0.0f, 0.0f);
        translateMatrix = IdentityMatrix;
        rotationMatrix = IdentityMatrix;
        scaleMatrix = IdentityMatrix;
    }

    Transform(const Vector3& position, const Matrix4& translateMatrix, const Matrix4& rotationMatrix, const Matrix4& scaleMatrix)
        : position(position), translateMatrix(translateMatrix), rotationMatrix(rotationMatrix), scaleMatrix(scaleMatrix) {
    }

    inline void Translate(const Vector3& translateVector) {
        // World space translation
        position += translateVector;
    }

    inline void Scale(const Vector3& scaleVector) {
        Matrix4 matrix = ScaleMatrix(scaleVector);
        scaleMatrix = matrix * scaleMatrix;
    }

    inline void RotateXAxis(float degreeRadians) {
        Matrix4 matrix = RotateMatrixXAxis(degreeRadians);
        rotationMatrix = matrix * rotationMatrix;
    }

    inline void RotateYAxis(float degreeRadians) {
        Matrix4 matrix = RotateMatrixYAxis(degreeRadians);
        rotationMatrix = matrix * rotationMatrix;
    }

    inline void RotateZAxis(float degreeRadians) {
        Matrix4 matrix = RotateMatrixZAxis(degreeRadians);
        rotationMatrix = matrix * rotationMatrix;
    }

    inline Matrix4 GetTransformMatrix() {
        Matrix4 translateMatrix = TranslateMatrix(position);
        return translateMatrix * rotationMatrix * scaleMatrix;
    }


public:
    Vector3 position;

    Matrix4 translateMatrix; // Currently we don't use this
    Matrix4 rotationMatrix;
    Matrix4 scaleMatrix;
};

