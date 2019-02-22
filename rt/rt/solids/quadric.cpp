#include "triangle.h"
#include <rt/bbox.h>
#include <rt/intersection.h>
#include <rt/solids/quadric.h>

namespace rt
{

Quadric::Quadric(
  const Quadric::Coefficients& cof, CoordMapper* texMapper, Material* material)
  : Solid(texMapper, material)
  , cof(cof)
{
}

BBox Quadric::getBounds() const
{
  // it's kind of hard to compute the bounds, so just overapproximate
  return BBox::full();
}

Intersection Quadric::intersect(
  const Ray& ray, float previousBestDistance) const
{
  const float alpha = cof.a * sqr(ray.d.x) + cof.b * sqr(ray.d.y) +
                      cof.c * sqr(ray.d.z) + cof.d * ray.d.x * ray.d.y +
                      cof.e * ray.d.x * ray.d.z + cof.f * ray.d.y * ray.d.z;
  const float beta =
    2 * (cof.a * ray.d.x * ray.o.x + cof.b * ray.d.y * ray.o.y +
          cof.c * ray.d.z * ray.o.z) +
    cof.d * (ray.o.x * ray.d.y + ray.d.x * ray.o.y) +
    cof.e * (ray.o.x * ray.d.z + ray.d.x * ray.o.z) +
    cof.f * (ray.o.y * ray.d.z + ray.d.y * ray.o.z) + cof.g * ray.d.x +
    cof.h * ray.d.y + cof.i * ray.d.z;
  const float gamma = cof.a * sqr(ray.o.x) + cof.b * sqr(ray.o.y) +
                      cof.c * sqr(ray.o.z) + cof.d * ray.o.x * ray.o.y +
                      cof.e * ray.o.x * ray.o.z + cof.f * ray.o.y * ray.o.z +
                      cof.g * ray.o.x + cof.h * ray.o.y + cof.i * ray.o.z +
                      cof.j;

  const float under_sqr = sqr(beta) - 4 * alpha * gamma;

  if(under_sqr < 0.f)
    return Intersection::failure();

  const float the_sqr = sqrt(under_sqr);
  const float t1      = (-beta + the_sqr) / (2 * alpha);
  const float t2      = (-beta - the_sqr) / (2 * alpha);

  if(std::min(t1, t2) > previousBestDistance)
    return Intersection::failure();

  const float t = std::min(t1, t2);
  return Intersection(
    t, ray, this, cof.normalAt(ray.getPoint(t)), Point(0, 0, 0));
}

Solid::Sample Quadric::sample() const
{
  NOT_IMPLEMENTED;
}

float Quadric::getArea() const
{
  // yet another overapproximation
  return FLT_MAX;
}
}
