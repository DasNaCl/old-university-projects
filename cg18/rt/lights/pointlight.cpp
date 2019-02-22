#include <core/scalar.h>
#include <rt/lights/pointlight.h>

namespace rt
{

PointLight::PointLight(const Point& position, const RGBColor& intensity)
  : pos(position)
  , intensity(intensity)
{
}

LightHit PointLight::getLightHit(const Point& p) const
{
  return { (pos - p).normalize(), (pos - p).length(), { 0, 0, 0 } };
}

RGBColor PointLight::getIntensity(const LightHit& irr) const
{
  return intensity / sqr(irr.distance);
}
}
