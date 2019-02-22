#include <core/assert.h>
#include <core/color.h>
#include <rt/integrators/castingdist.h>
#include <rt/intersection.h>
#include <rt/world.h>

namespace rt
{

RayCastingDistIntegrator::RayCastingDistIntegrator(World* world,
  const RGBColor& nearColor, float nearDist, const RGBColor& farColor,
  float farDist)
  : Integrator(world)
  , nearColor(nearColor)
  , farColor(farColor)
  , nearDist(nearDist)
  , farDist(farDist)
{
}

RGBColor RayCastingDistIntegrator::getRadiance(const Ray& ray) const
{
  Intersection intersection = world->scene->intersect(ray);
  if(!intersection)
    return RGBColor::rep(0.f);

  assert(std::abs(farDist - nearDist) > rt::epsilon);
  const float percentage = std::max(0.f,
    std::min(1.f, (intersection.distance - nearDist) / (farDist - nearDist)));

  const RGBColor interColor =
    (1.f - percentage) * nearColor + percentage * farColor;


  float cos = dot(-ray.d, intersection.normal());

  return (std::max(0.f, cos)) * interColor;
}
}
