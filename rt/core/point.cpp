#include <core/assert.h>
#include <core/float4.h>
#include <core/point.h>
#include <core/scalar.h>
#include <core/vector.h>

#include <algorithm>
#include <cmath>

namespace rt
{

Point::Point(const Float4& f4)
  : x()
  , y()
  , z()
{
  //assert(std::fabs(f4[3] - 1.f) < rt::epsilon);
  if(f4[3] == 0)
  {
    x = f4[0];
    y = f4[1];
    z = f4[2];
  }
  else
  {
    x = f4[0] / f4[3];
    y = f4[1] / f4[3];
    z = f4[2] / f4[3];
  }
}

Vector Point::operator-(const Point& b) const
{
  return Vector(x - b.x, y - b.y, z - b.z);
}

bool Point::operator==(const Point& b) const
{
  return std::fabs(x - b.x) < rt::epsilon && std::fabs(y - b.y) < rt::epsilon &&
         std::fabs(z - b.z) < rt::epsilon;
}

bool Point::operator!=(const Point& b) const
{
  return std::fabs(x - b.x) > rt::epsilon || std::fabs(y - b.y) > rt::epsilon ||
         std::fabs(z - b.z) > rt::epsilon;
}

Point operator*(float scalar, const Point& b)
{
  return Point(scalar * b.x, scalar * b.y, scalar * b.z);
}

Point operator*(const Point& a, float scalar)
{
  return scalar * a;
}

Point min(const Point& a, const Point& b)
{
  return Point(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
}

Point max(const Point& a, const Point& b)
{
  return Point(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
}
}
