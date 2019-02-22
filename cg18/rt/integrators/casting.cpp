#include <core/assert.h>
#include <core/color.h>
#include <rt/integrators/casting.h>
#include <rt/intersection.h>
#include <rt/world.h>

namespace rt
{

RGBColor RayCastingIntegrator::getRadiance(const Ray& ray) const
{
  auto intersection = world->scene->intersect(ray);

  if(!intersection)
    return RGBColor::rep(0.f);
  float cos = dot(ray.d, intersection.normal());
  return RGBColor::rep(std::abs(std::min(0.f, cos)));
}
}
