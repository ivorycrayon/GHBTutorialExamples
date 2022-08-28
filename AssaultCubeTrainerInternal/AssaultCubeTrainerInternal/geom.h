#pragma once
#include <algorithm>

class Vec3
{
public:

    float x, y, z;

    Vec3() {};
    Vec3(const float x, const float y, const float z) : x(x), y(y), z(z) {}
    Vec3 operator + (const Vec3& rhs) const { return Vec3(x + rhs.x, y + rhs.y, z + rhs.z); }
    Vec3 operator - (const Vec3& rhs) const { return Vec3(x - rhs.x, y - rhs.y, z - rhs.z); }
    Vec3 operator * (const float& rhs) const { return Vec3(x * rhs, y * rhs, z * rhs); }
    Vec3 operator / (const float& rhs) const { return Vec3(x / rhs, y / rhs, z / rhs); }
    Vec3& operator += (const Vec3& rhs) { return *this = *this + rhs; }
    Vec3& operator -= (const Vec3& rhs) { return *this = *this - rhs; }
    Vec3& operator *= (const float& rhs) { return *this = *this * rhs; }
    Vec3& operator /= (const float& rhs) { return *this = *this / rhs; }
    float Length() const { return sqrtf(x * x + y * y + z * z); }
    Vec3 Normalize() const { return *this * (1 / Length()); }
    float Distance(const Vec3& rhs) const { return (*this - rhs).Length(); }
};

struct Vec4
{
    float x, y, z, w;
};

using Vector3 = Vec3;
using vector3 = Vec3;
using vec3 = Vec3;
using Vec = Vec3;

bool WorldToScreen(Vec3 pos, Vec3& screen, float matrix[16], int windowWidth, int windowHeight);