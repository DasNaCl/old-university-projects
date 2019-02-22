#include <cmath>
#include <rt/lights/spotlight.h>

namespace rt
{

SpotLight::SpotLight(const Point& position, const Vector& direction,
  float angle, float exp, const RGBColor& intensity)
  : pos(position)
  , dir(direction)
  , angle(angle)
  , cosphi(std::cos(angle))
  , exp(exp)
  , intensity(intensity)
{
}

LightHit SpotLight::getLightHit(const Point& p) const
{
  return { (pos - p).normalize(), (pos - p).length() };
}

RGBColor SpotLight::getIntensity(const LightHit& irr) const
{
  const float dott = dot(dir, -irr.direction.normalize());

  if(dott < cosphi)
    return RGBColor::rep(0.f);
  return intensity * (std::pow(std::abs(dott), exp) / sqr(irr.distance));
}
}
