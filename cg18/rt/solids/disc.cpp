#include <cmath>
#include <core/assert.h>
#include <rt/bbox.h>
#include <rt/intersection.h>
#include <rt/solids/disc.h>

namespace rt
{

Disc::Disc(const Point& center, const Vector& normal, float radius,
  CoordMapper* texMapper, Material* material)
  : Solid(texMapper, material)
  , center(center)
  , normal(normal.normalize())
  , radius(radius)
{
}

BBox Disc::getBounds() const
{
  assert(radius > 0);

  // yet another overapproximation
  return BBox(center - Vector::rep(radius), center + Vector::rep(radius));
}

Intersection Disc::intersect(const Ray& ray, float previousBestDistance) const
{
  float ndir = dot(ray.d, normal);
  if(std::abs(ndir) < rt::epsilon)
  {
    return Intersection::failure();
  }
  float dist = dot((center - ray.o), normal) / ndir;
  Point p    = ray.getPoint(dist);

  if((p - center).length() <= radius && dist > 0 && previousBestDistance > dist)
  {
    // is the local vector correct? I dunno, but it is something
    return Intersection(dist, ray, this, normal.normalize(), p);
  }
  return Intersection::failure();
}

Solid::Sample Disc::sample() const
{
  NOT_IMPLEMENTED;
}

float Disc::getArea() const
{
  return pi * sqr(radius);
}
}
