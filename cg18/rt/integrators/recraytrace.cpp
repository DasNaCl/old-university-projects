#include <core/color.h>
#include <rt/coordmappers/world.h>
#include <rt/integrators/recraytrace.h>
#include <rt/intersection.h>
#include <rt/lights/light.h>
#include <rt/materials/material.h>
#include <rt/materials/lambertian.h>
#include <rt/textures/constant.h>
#include <rt/solids/solid.h>
#include <rt/world.h>

namespace rt
{
static thread_local auto defaultMapper = WorldMapper();
static thread_local auto defaultMaterial = LambertianMaterial(new ConstantTexture(RGBColor::rep(0.f)),
                                                              new ConstantTexture(RGBColor::rep(1.f)));

RGBColor RecursiveRayTracingIntegrator::getRadiance(const Ray& ray) const
{
  return getRadiance(ray, 0);
}
RGBColor RecursiveRayTracingIntegrator::getRadiance(const Ray& ray, size_t recD) const{

  Intersection intersection = this->world->scene->intersect(ray);

  RGBColor intensity, reflectance;
  RGBColor color = {0,0,0};

  if(!intersection)
    return color;

  auto&      material            = *(intersection.solid->material ? intersection.solid->material : (Material*)&defaultMaterial);
  Material::Sampling sample = material.useSampling();
  const auto interPoint = intersection.hitPoint();
  const auto intNorm             = intersection.normal().normalize();
  const auto pointOfIntersection = intersection.hitPoint();

  const auto texPoint =
    intersection.solid->texMapper
      ? intersection.solid->texMapper->getCoords(intersection)
      : defaultMapper.getCoords(intersection);

  color = material.getEmission(
    texPoint, intersection.normal().normalize(), ray.d.normalize());
  if(sample == Material::SAMPLING_SECONDARY ||
     sample == Material::SAMPLING_NOT_NEEDED)
  {
    for(auto& light : world->light)
    {
      const auto lightHit = light->getLightHit(interPoint);
      const auto lHitNdir = lightHit.direction.normalize();
      const float tmp0 = dot(lHitNdir, intNorm);
      const float tmp1     = dot(-ray.d.normalize(), intNorm);

      if((tmp0 < 0 && tmp1 > 0) || (tmp0 > 0 && tmp1 < 0))
        continue;

      auto shadowIntersection = world->scene->intersect(
        //something goes wrong with the upset point
        { pointOfIntersection - 0.0001 * -lHitNdir, lHitNdir },
        lightHit.distance - rt::epsilon);
      if(shadowIntersection)
        continue;

      if(dot(-lightHit.direction, intersection.normal().normalize()) *
           dot(ray.d.normalize(), intersection.normal().normalize()) <
         0)
        continue;

      intensity = light->getIntensity(lightHit);
      reflectance = material.getReflectance(
        texPoint, intNorm, (lHitNdir.normalize()), ray.d.normalize());

      color = color + (intensity * reflectance);
    }
  }
  if(recD < recursion_depth && (sample == Material::SAMPLING_SECONDARY ||
                                 sample == Material::SAMPLING_ALL))
  {
    auto r = material.getSampleReflectance(interPoint,intersection.normal(), ray.d);
    auto ref =
      getRadiance({ interPoint + (0.01 * r.direction), r.direction }, recD + 1);
    color = color + r.reflectance * ref;
  }
  return color;
}
}
