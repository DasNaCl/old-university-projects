#include <core/scalar.h>
#include <rt/lights/directional.h>

namespace rt
{

DirectionalLight::DirectionalLight(
  const Vector& direction, const RGBColor& color)
  : dir(direction.normalize())
  , col(color)
{
}

LightHit DirectionalLight::getLightHit(const Point&) const
{
  return { -dir, FLT_MAX };
}

RGBColor DirectionalLight::getIntensity(const LightHit&) const
{
  return col;
}
}
