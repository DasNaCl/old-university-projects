#include <core/color.h>
#include <rt/coordmappers/world.h>
#include <rt/integrators/volumetracer.h>
#include <rt/intersection.h>
#include <rt/lights/light.h>
#include <rt/materials/material.h>
#include <rt/solids/solid.h>
#include <rt/volumes/volume.h>
#include <rt/world.h>

namespace rt
{
static thread_local auto defaultMapper = WorldMapper();

RGBColor VolumeIntegrator::getRadiance(const Ray& ray) const
{
  auto intersection = world->scene->intersect(ray);
  if(!intersection)
  {
    if(world->volume)
      return rayMarch(
        RGBColor::rep(0.f), RGBColor::rep(1.f), maxRayMarchingLen, ray, true);
    else
      return RGBColor::rep(0.f);
  }

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

    float attenuation = 1.f;
    if(world->volume && world->volume->isHeterogenous())
      col =
        col + reflectance * rayMarch(intensity, RGBColor::rep(1.f),
                              intersection.distance, intersection.ray, false);
    else
      col = col + intensity * reflectance * attenuation;
  }
  const auto finalCol = col + material.getEmission(texPoint, intersection.normal(), ray.d.normalize());
  if(world->volume)
  {
    if(world->volume->isLitByAmbient())
    {
      const float attenuation =
        (world->volume ? world->volume->getTransmittance(intersection.distance)
                       : 1.f);
      return attenuation * finalCol +
             (1.f - attenuation) * world->volume->getColor(pointOfIntersection);
    }
    else
      return rayMarch(finalCol, RGBColor::rep(1.f), intersection.distance,
        intersection.ray, true);
  }
  return finalCol;
}

RGBColor VolumeIntegrator::rayMarch(const RGBColor& col, RGBColor T,
  float distance, const Ray& ray, bool light) const
{
  distance = std::max(distance, maxRayMarchingLen);

  RGBColor L = RGBColor::rep(0.f);

  for(float t = 0.f; t < distance; t += rayMarchingStep)
  {
    const float d                  = std::min(distance - t, rayMarchingStep);
    const float deltaTransmittance = world->volume->getTransmittance(d);

    RGBColor    delta = RGBColor::rep(0.f);
    const Point p     = ray.o + ray.d * (t + d / 2.f);
    if(light)
    {
      for(auto& light : world->light)
      {
        const auto lightHit = light->getLightHit(p);
        const auto lHitNdir = lightHit.direction.normalize();

        auto shadowIntersection =
          world->scene->intersect({ p, lHitNdir }, lightHit.distance);
        if(shadowIntersection)
          continue;

        const float transmittance =
          world->volume->getTransmittance(lightHit.distance);
        const auto irradiance = light->getIntensity(lightHit) * transmittance;

        delta = delta + irradiance;
      }
    }

    L = L + T * (1.f - deltaTransmittance) * delta * world->volume->getColor(p);
    T = T * deltaTransmittance;
  }

  return col * T + L;
}
}
