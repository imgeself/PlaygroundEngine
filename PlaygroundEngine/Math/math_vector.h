#ifndef _VECTOR_H_
#define _VECTOR_H_

// TODO: Make flexible vector class using templates someday!
#include <stdint.h>
#include "../Core.h"

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
struct PG_API Vector2 {
    float x, y = 0.0f;

    Vector2();
    Vector2(float x, float y);
    Vector2(const Vector2& v);

    float& operator[](const uint32_t index);
    Vector2 operator-();
    Vector2 operator=(const Vector2 v);
    Vector2 operator+=(const Vector2 v);
    Vector2 operator*=(const Vector2 v);
    Vector2 operator-=(const Vector2 v);
    Vector2 operator/=(const Vector2 v);
    Vector2 operator-(const Vector2 v);
    Vector2 operator-(const Vector2 v) const;
    Vector2 operator*(const Vector2 v);
    Vector2 operator*(const Vector2 v) const;
    Vector2 operator/(const Vector2 v);
    Vector2 operator/(const Vector2 v) const;
    Vector2 operator+(const Vector2 v);
    Vector2 operator+(const Vector2 v) const;

    Vector2 operator*(const float factor);
    Vector2 operator+(const float factor);
    Vector2 operator-(const float factor);
    Vector2 operator/(const float factor);

    bool operator==(const Vector2 v);
    bool operator!=(const Vector2 v);
};

inline Vector2::Vector2() {
}

inline Vector2::Vector2(float x, float y) {
    this->x = x;
    this->y = y;
}

inline Vector2::Vector2(const Vector2& v) {
    this->x = v.x;
    this->y = v.y;
}

inline float& Vector2::operator[](const uint32_t index) {
    return (&x)[index];
}

inline Vector2 Vector2::operator-() {
    return Vector2(-x, -y);
}

inline Vector2 Vector2::operator=(const Vector2 v) {
    x = v.x;
    y = v.y;
    return *this;
}

inline Vector2 Vector2::operator+=(const Vector2 v) {
    x += v.x;
    y += v.y;
    return *this;
}

inline Vector2 Vector2::operator*=(const Vector2 v) {
    x *= v.x;
    y *= v.y;
    return *this;
}

inline Vector2 Vector2::operator-=(const Vector2 v) {
    x -= v.x;
    y -= v.y;
    return *this;
}

inline Vector2 Vector2::operator/=(const Vector2 v) {
    x /= v.x;
    y /= v.y;
    return *this;
}

inline Vector2 Vector2::operator*(const Vector2 v) {
    return Vector2(x * v.x, y * v.y);
}

inline Vector2 Vector2::operator*(const Vector2 v) const {
    return Vector2(x * v.x, y * v.y);
}

inline Vector2 Vector2::operator/(const Vector2 v) {
    return Vector2(x / v.x, y / v.y);
}

inline Vector2 Vector2::operator/(const Vector2 v) const {
    return Vector2(x / v.x, y / v.y);
}

inline Vector2 Vector2::operator+(const Vector2 v) {
    return Vector2(x + v.x, y + v.y);
}

inline Vector2 Vector2::operator+(const Vector2 v) const {
    return Vector2(x + v.x, y + v.y);
}

inline Vector2 Vector2::operator-(const Vector2 v) {
    return Vector2(x - v.x, y - v.y);
}

inline Vector2 Vector2::operator-(const Vector2 v) const {
    return Vector2(x - v.x, y - v.y);
}

inline Vector2 Vector2::operator*(const float factor) {
    return Vector2(x * factor, y * factor);
}

inline Vector2 Vector2::operator+(const float factor) {
    return Vector2(x + factor, y + factor);
}

inline Vector2 Vector2::operator-(const float factor) {
    return Vector2(x - factor, y - factor);
}

inline Vector2 Vector2::operator/(const float factor) {
    return Vector2(x / factor, y / factor);
}

inline bool Vector2::operator==(const Vector2 v) {
    return ((x == v.x) && (y == v.y));
}

inline bool Vector2::operator!=(const Vector2 v) {
    return ((x != v.x) || (y != v.y));
}

inline float DotProduct(const Vector2 v1, const Vector2 v2) {
    return v1.x * v2.x + v1.y * v2.y;
}

inline float Lenght(const Vector2 v) {
    return sqrtf(DotProduct(v, v));
}

inline Vector2 Normalize(const Vector2 v) {
    const float dot = DotProduct(v, v);
    const float factor = 1 / sqrtf(dot);
    return Vector2(v.x * factor, v.y * factor);
}

inline Vector2 Floor(const Vector2& v) {
    return Vector2(floorf(v.x), floorf(v.y));
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
struct PG_API Vector3 {
    float x,y,z = 0.0f;
    
    Vector3();
    Vector3(float x, float y, float z);
    Vector3(const Vector3 &v);

    Vector2 xy();
    
    float& operator[](const uint32_t index);
    Vector3 operator-();
    Vector3 operator=(const Vector3 v);
    Vector3 operator+=(const Vector3 v);
    Vector3 operator*=(const Vector3 v);
    Vector3 operator-=(const Vector3 v);
    Vector3 operator/=(const Vector3 v);
    Vector3 operator-(const Vector3 v);
    Vector3 operator-(const Vector3 v) const;
    Vector3 operator*(const Vector3 v);
    Vector3 operator*(const Vector3 v) const;
    Vector3 operator/(const Vector3 v);
    Vector3 operator/(const Vector3 v) const;
    Vector3 operator+(const Vector3 v);
    Vector3 operator+(const Vector3 v) const;
    
    Vector3 operator*(const float factor);
    Vector3 operator+(const float factor);
    Vector3 operator-(const float factor);
    Vector3 operator/(const float factor);

    bool operator==(const Vector3 v);
    bool operator!=(const Vector3 v);
};

inline Vector3::Vector3() {
}

inline Vector3::Vector3(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

inline Vector3::Vector3(const Vector3 &v) {
    this->x = v.x;
    this->y = v.y;
    this->z = v.z;
}

inline Vector2 Vector3::xy() {
    return Vector2(this->x, this->y);
}

inline float& Vector3::operator[](const uint32_t index) {
    return (&x)[index];
}

inline Vector3 Vector3::operator-() {
    return Vector3(-x, -y, -z);
}

inline Vector3 Vector3::operator=(const Vector3 v) {
    x = v.x;
    y = v.y;
    z = v.z;
    return *this;
}

inline Vector3 Vector3::operator+=(const Vector3 v) {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

inline Vector3 Vector3::operator*=(const Vector3 v) {
    x *= v.x;
    y *= v.y;
    z *= v.z;
    return *this;
}

inline Vector3 Vector3::operator-=(const Vector3 v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

inline Vector3 Vector3::operator/=(const Vector3 v) {
    x /= v.x;
    y /= v.y;
    z /= v.z;
    return *this;
}

inline Vector3 Vector3::operator*(const Vector3 v) {
    return Vector3(x * v.x, y * v.y, z * v.z);
}

inline Vector3 Vector3::operator*(const Vector3 v) const {
    return Vector3(x * v.x, y * v.y, z * v.z);
}

inline Vector3 Vector3::operator/(const Vector3 v) {
    return Vector3(x / v.x, y / v.y, z / v.z);
}

inline Vector3 Vector3::operator/(const Vector3 v) const {
    return Vector3(x / v.x, y / v.y, z / v.z);
}

inline Vector3 Vector3::operator+(const Vector3 v) {
    return Vector3(x + v.x, y + v.y, z + v.z);
}

inline Vector3 Vector3::operator+(const Vector3 v) const {
    return Vector3(x + v.x, y + v.y, z + v.z);
}

inline Vector3 Vector3::operator-(const Vector3 v) {
    return Vector3(x - v.x, y - v.y, z - v.z);
}

inline Vector3 Vector3::operator-(const Vector3 v) const {
    return Vector3(x - v.x, y - v.y, z - v.z);
}

inline Vector3 Vector3::operator*(const float factor) {
    return Vector3(x * factor, y * factor, z * factor);
}

inline Vector3 Vector3::operator+(const float factor) {
    return Vector3(x + factor, y + factor, z + factor);
}

inline Vector3 Vector3::operator-(const float factor) {
    return Vector3(x - factor, y - factor, z - factor);
}

inline Vector3 Vector3::operator/(const float factor) {
    return Vector3(x / factor, y / factor, z / factor);
}

inline bool Vector3::operator==(const Vector3 v) {
    return ((x == v.x) && (y == v.y) && (z == v.z));
}

inline bool Vector3::operator!=(const Vector3 v) {
    return ((x != v.x) || (y != v.y) || (z != v.z));
} 

inline float DotProduct(const Vector3 v1, const Vector3 v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline Vector3 CrossProduct(const Vector3 v1, const Vector3 v2) {
    return Vector3(v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x);
}

inline float Lenght(const Vector3 v) {
    return sqrtf(DotProduct(v, v));
}

inline Vector3 Normalize(const Vector3 v) {
    const float dot = DotProduct(v, v);
    const float factor = 1 / sqrtf(dot);
    return Vector3(v.x * factor, v.y * factor, v.z * factor);
}

inline Vector3 Floor(const Vector3& v) {
    return Vector3(floorf(v.x), floorf(v.y), floorf(v.z));
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
struct PG_API Vector4 {
    float x, y, z, w = 0.0f;

    Vector4();
    Vector4(float x, float y, float z, float w);
    Vector4(Vector3 v, float w);
    Vector4(const Vector4& v);

    Vector3 xyz();

    float& operator[](const uint32_t index);
    Vector4 operator-();
    Vector4 operator=(const Vector4 v);
    Vector4 operator+=(const Vector4 v);
    Vector4 operator*=(const Vector4 v);
    Vector4 operator-=(const Vector4 v);
    Vector4 operator/=(const Vector4 v);
    Vector4 operator-(const Vector4 v);
    Vector4 operator-(const Vector4 v) const;
    Vector4 operator*(const Vector4 v);
    Vector4 operator*(const Vector4 v) const;
    Vector4 operator/(const Vector4 v);
    Vector4 operator/(const Vector4 v) const;
    Vector4 operator+(const Vector4 v);
    Vector4 operator+(const Vector4 v) const;

    Vector4 operator*(const float factor);
    Vector4 operator+(const float factor);
    Vector4 operator-(const float factor);
    Vector4 operator/(const float factor);

    bool operator==(const Vector4 v);
    bool operator!=(const Vector4 v);
};

inline Vector4::Vector4() {
}

inline Vector4::Vector4(float x, float y, float z, float w) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}

inline Vector4::Vector4(Vector3 v, float w) {
    this->x = v.x;
    this->y = v.y;
    this->z = v.z;
    this->w = w;
}

inline Vector4::Vector4(const Vector4& v) {
    this->x = v.x;
    this->y = v.y;
    this->z = v.z;
    this->w = v.w;
}

inline float& Vector4::operator[](const uint32_t index) {
    return (&x)[index];
}

inline Vector3 Vector4::xyz() {
    return Vector3(x, y, z);
}

inline Vector4 Vector4::operator-() {
    return Vector4(-x, -y, -z, -w);
}

inline Vector4 Vector4::operator=(const Vector4 v) {
    x = v.x;
    y = v.y;
    z = v.z;
    w = v.w;
    return *this;
}

inline Vector4 Vector4::operator+=(const Vector4 v) {
    x += v.x;
    y += v.y;
    z += v.z;
    w += v.w;
    return *this;
}

inline Vector4 Vector4::operator*=(const Vector4 v) {
    x *= v.x;
    y *= v.y;
    z *= v.z;
    w *= v.w;
    return *this;
}

inline Vector4 Vector4::operator-=(const Vector4 v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    w -= v.w;
    return *this;
}

inline Vector4 Vector4::operator/=(const Vector4 v) {
    x /= v.x;
    y /= v.y;
    z /= v.z;
    w /= v.w;
    return *this;
}

inline Vector4 Vector4::operator*(const Vector4 v) {
    return Vector4(x * v.x, y * v.y, z * v.z, w * v.w);
}

inline Vector4 Vector4::operator*(const Vector4 v) const {
    return Vector4(x * v.x, y * v.y, z * v.z, w * v.w);
}

inline Vector4 Vector4::operator/(const Vector4 v) {
    return Vector4(x / v.x, y / v.y, z / v.z, w / v.w);
}

inline Vector4 Vector4::operator/(const Vector4 v) const {
    return Vector4(x / v.x, y / v.y, z / v.z, w / v.w);
}

inline Vector4 Vector4::operator+(const Vector4 v) {
    return Vector4(x + v.x, y + v.y, z + v.z, w + v.w);
}

inline Vector4 Vector4::operator+(const Vector4 v) const {
    return Vector4(x + v.x, y + v.y, z + v.z, w + v.w);
}

inline Vector4 Vector4::operator-(const Vector4 v) {
    return Vector4(x - v.x, y - v.y, z - v.z, w - v.w);
}

inline Vector4 Vector4::operator-(const Vector4 v) const {
    return Vector4(x - v.x, y - v.y, z - v.z, w - v.w);
}

inline Vector4 Vector4::operator*(const float factor) {
    return Vector4(x * factor, y * factor, z * factor, w * factor);
}

inline Vector4 Vector4::operator+(const float factor) {
    return Vector4(x + factor, y + factor, z + factor, w + factor);
}

inline Vector4 Vector4::operator-(const float factor) {
    return Vector4(x - factor, y - factor, z - factor, w - factor);
}

inline Vector4 Vector4::operator/(const float factor) {
    return Vector4(x / factor, y / factor, z / factor, w / factor);
}

inline bool Vector4::operator==(const Vector4 v) {
    return ((x == v.x) && (y == v.y) && (z == v.z) && (w == v.w));
}

inline bool Vector4::operator!=(const Vector4 v) {
    return ((x != v.x) || (y != v.y) || (z != v.z) || (w != v.w));
}

inline float DotProduct(const Vector4 v1, const Vector4 v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

inline float Lenght(const Vector4 v) {
    return sqrtf(DotProduct(v, v));
}

inline Vector4 Normalize(const Vector4 v) {
    const float dot = DotProduct(v, v);
    const float factor = 1 / sqrtf(dot);
    return Vector4(v.x * factor, v.y * factor, v.z * factor, v.w * factor);
}

inline Vector4 Floor(const Vector4& v) {
    return Vector4(floorf(v.x), floorf(v.y), floorf(v.z), floorf(v.w));
}

#endif
