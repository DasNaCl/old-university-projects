#include <algorithm>
#include <cmath>
#include <core/assert.h>
#include <core/float4.h>
#include <core/point.h>
#include <core/random.h>
#include <core/scalar.h>
#include <core/vector.h>

namespace rt
{

Vector::Vector(const Float4& f4)
  : x(f4[0])
  , y(f4[1])
  , z(f4[2])
{
}

Vector Vector::random()
{
  return Vector(rt::random(), rt::random(), rt::random());
}

Vector Vector::operator+(const Vector& b) const
{
  return Vector(x + b.x, y + b.y, z + b.z);
}

Vector Vector::operator-(const Vector& b) const
{
  return Vector(x - b.x, y - b.y, z - b.z);
}

Vector Vector::normalize() const
{
  const float len = length();
  return Vector(x / len, y / len, z / len);
}

Vector Vector::operator-() const
{
  return Vector(-x, -y, -z);
}

float Vector::lensqr() const
{
  return dot(*this, *this);
}

float Vector::length() const
{
  return std::sqrt(lensqr());
}

bool Vector::operator==(const Vector& b) const
{
  return std::fabs(x - b.x) < 0.000001f && std::fabs(y - b.y) < 0.000001f &&
         std::fabs(z - b.z) < 0.000001f;
}

bool Vector::operator!=(const Vector& b) const
{
  return std::fabs(x - b.x) > 0.000001f || std::fabs(y - b.y) > 0.000001f ||
         std::fabs(z - b.z) > 0.000001f;
}

Vector operator*(float scalar, const Vector& b)
{
  return Vector(scalar * b.x, scalar * b.y, scalar * b.z);
}

Vector operator*(const Vector& a, float scalar)
{
  return scalar * a;
}

Vector operator/(const Vector& a, float scalar)
{
  return Vector(a.x / scalar, a.y / scalar, a.z / scalar);
}

Vector cross(const Vector& a, const Vector& b)
{
  return Vector(
    a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

float dot(const Vector& a, const Vector& b)
{
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector min(const Vector& a, const Vector& b)
{
  return Vector(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
}

Vector max(const Vector& a, const Vector& b)
{
  return Vector(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
}

Point operator+(const Point& a, const Vector& b)
{
  return Point(a.x + b.x, a.y + b.y, a.z + b.z);
}

Point operator+(const Vector& a, const Point& b)
{
  return b + a;
}

Point operator-(const Point& a, const Vector& b)
{
  return Point(a.x - b.x, a.y - b.y, a.z - b.z);
}

Point operator*(const Float4& scale, const Point& p)
{
  NOT_IMPLEMENTED;
}
}
