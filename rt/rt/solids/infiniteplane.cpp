#include <core/assert.h>
#include <rt/bbox.h>
#include <rt/intersection.h>
#include <rt/solids/infiniteplane.h>

namespace rt
{

InfinitePlane::InfinitePlane(const Point& origin, const Vector& normal,
  CoordMapper* texMapper, Material* material)
  : Solid(texMapper, material)
  , origin(origin)
  , normal(normal)
{
}

BBox InfinitePlane::getBounds() const
{
  // plane is infinite, there is no way to put it in a bounding box :(
  return BBox::full();
}

Intersection InfinitePlane::intersect(
  const Ray& ray, float previousBestDistance) const
{
  const auto norm = normal.normalize();
  float      ndir = dot(ray.d, norm);
  float      num  = dot((origin - ray.o), norm);
  if(std::abs(ndir) >= rt::epsilon)
  {
    float dist = num / ndir;
    if(dist < previousBestDistance && dist > 0)
      return {dist, ray, this, norm, ray.getPoint(dist)};
  }
  return Intersection::failure();
}


Solid::Sample InfinitePlane::sample() const
{
  NOT_IMPLEMENTED;
}

float InfinitePlane::getArea() const
{
  return FLT_MAX;
}
}
