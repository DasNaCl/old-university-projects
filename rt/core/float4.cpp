//
// Created by Matthias on 20.11.2018.
//
#include "scalar.h"
#include <algorithm>
#include <cmath>
#include <core/assert.h>
#include <core/float4.h>
#include <core/point.h>
#include <core/vector.h>

namespace rt
{
Float4::Float4(float x, float y, float z, float w)
  : coords{ x, y, z, w }
{
}
Float4::Float4(const Point& p)
  : coords{ p.x, p.y, p.z, 1.f }
{
}
Float4::Float4(const Vector& v)
  : coords{ v.x, v.y, v.z, 0.f }
{
}

float& Float4::operator[](int idx)
{
  assert(0 <= idx && idx < 4);
  return coords[idx];
}
float Float4::operator[](int idx) const
{
  assert(0 <= idx && idx < 4);
  return coords[idx];
}

Float4 Float4::operator+(const Float4& b) const
{
  return Float4(
    coords[0] + b[0], coords[1] + b[1], coords[2] + b[2], coords[3] + b[3]);
}

Float4 Float4::operator-(const Float4& b) const
{
  return Float4(
    coords[0] - b[0], coords[1] - b[1], coords[2] - b[2], coords[3] - b[3]);
}
Float4 Float4::operator*(const Float4& b) const
{
  return Float4(
    coords[0] * b[0], coords[1] * b[1], coords[2] * b[2], coords[3] * b[3]);
}
Float4 Float4::operator/(const Float4& b) const
{
  return Float4(
    coords[0] / b[0], coords[1] / b[1], coords[2] / b[2], coords[3] / b[3]);
}


Float4 Float4::operator-() const
{
  return Float4(-coords[0], -coords[1], -coords[2], -coords[3]);
}

bool Float4::operator==(const Float4& b) const
{
  return std::fabs(coords[0] - b[0]) < rt::epsilon &&
         std::fabs(coords[1] - b[1]) < rt::epsilon &&
         std::fabs(coords[2] - b[2]) < rt::epsilon &&
         std::fabs(coords[3] - b[3]) < rt::epsilon;
}
bool Float4::operator!=(const Float4& b) const
{
  return std::fabs(coords[0] - b[0]) > rt::epsilon ||
         std::fabs(coords[1] - b[1]) > rt::epsilon ||
         std::fabs(coords[2] - b[2]) > rt::epsilon ||
         std::fabs(coords[3] - b[3]) > rt::epsilon;
}


Float4 operator*(float scalar, const Float4& b)
{
  return Float4(b[0] * scalar, b[1] * scalar, b[2] * scalar, b[3] * scalar);
}
Float4 operator*(const Float4& a, float scalar)
{
  return Float4(a[0] * scalar, a[1] * scalar, a[2] * scalar, a[3] * scalar);
}
Float4 operator/(const Float4& a, float scalar)
{
  return Float4(a[0] / scalar, a[1] / scalar, a[2] / scalar, a[3] / scalar);
}
float dot(const Float4& a, const Float4& b)
{
  return a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3];
}

Float4 min(const Float4& a, const Float4& b)
{
  return Float4(std::min(a[0], b[0]), std::min(a[1], b[1]),
    std::min(a[2], b[2]), std::min(a[3], b[3]));
}
Float4 max(const Float4& a, const Float4& b)
{
  return Float4(std::max(a[0], b[0]), std::max(a[1], b[1]),
    std::max(a[2], b[2]), std::max(a[3], b[3]));
}
}
