#include <core/color.h>
#include <rt/coordmappers/world.h>
#include <rt/integrators/raytrace.h>
#include <rt/intersection.h>
#include <rt/lights/light.h>
#include <rt/materials/material.h>
#include <rt/solids/solid.h>
#include <rt/world.h>

namespace rt
{
static thread_local auto defaultMapper = WorldMapper();

RGBColor RayTracingIntegrator::getRadiance(const Ray& ray) const
{
  auto intersection = world->scene->intersect(ray);
  if(!intersection)
    return RGBColor::rep(0.f);

  auto&      material            = *(intersection.solid->material);
  auto       col                 = RGBColor::rep(0.f);
  const auto intNorm             = intersection.normal().normalize();
  const auto pointOfIntersection = intersection.hitPoint();

  const auto texPoint =
    intersection.solid->texMapper
      ? intersection.solid->texMapper->getCoords(intersection)
      : defaultMapper.getCoords(intersection);
  for(auto& light : world->light)
  {
    const auto lightHit = light->getLightHit(pointOfIntersection);
    const auto lHitNdir = lightHit.direction.normalize();

    const float tmp0 = dot(lHitNdir, intNorm);
    const float tmp1 = dot(-ray.d, intNorm);
    if((tmp0 < 0 && tmp1 > 0) || (tmp0 > 0 && tmp1 < 0))
      continue;  // different exit

    auto shadowIntersection = world->scene->intersect(
      { pointOfIntersection - 0.001 * -lHitNdir, lHitNdir },
      lightHit.distance - rt::epsilon);
    if(shadowIntersection)
      continue;

    const auto intensity   = light->getIntensity(lightHit);
    const auto reflectance = material.getReflectance(
      texPoint, intersection.normal(), ray.d, -lHitNdir);

    col = col + intensity * reflectance;
  }
  return col + material.getEmission(
                 texPoint, intersection.normal(), ray.d.normalize());
}
}
