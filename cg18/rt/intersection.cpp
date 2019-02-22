#include <core/assert.h>
#include <rt/intersection.h>

#include <cmath>
#include <limits>

namespace rt
{

Intersection Intersection::failure()
{
  auto i = Intersection();

  i.solid    = nullptr;
  i.distance = std::numeric_limits<float>::infinity();

  return i;
}

Intersection::Intersection(float distance, const Ray& ray, const Solid* solid,
  const Vector& normal, const Point& uv)
  : ray(ray)
  , solid(solid)
  , distance(distance)
  , normalVec(normal.normalize())
  , uv(uv)
{
}

Point Intersection::hitPoint() const
{
  return ray.getPoint(distance);
}

Vector Intersection::normal() const
{
  return normalVec;
}

void Intersection::updateNormal(Vector&& v)
{
  normalVec = v;
}

Point Intersection::local() const
{
  return uv;
}

Intersection::operator bool()
{
  return solid != nullptr;
}
}
