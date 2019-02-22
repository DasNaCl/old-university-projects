#include <cmath>
#include <rt/lights/perlinspotlight.h>
#include <rt/textures/perlin.h>

namespace rt
{

    PerlinSpotLight::PerlinSpotLight(const Point& position, const Vector& direction,
  float angle, float exp, const RGBColor& intensity)
  : pos(position)
  , dir(direction.normalize())
  , angle(angle)
  , cosphi(std::cos(angle))
  , exp(exp)
  , intensity(intensity)
{
  perlin =
          new PerlinTexture(RGBColor(10,10,10), RGBColor::rep(0.f));
  //perlin->addOctave(0.5f, 5.0f);
  //perlin->addOctave(0.25f, 10.0f);
  //perlin->addOctave(0.125f, 20.0f);
  perlin->addOctave(0.125f, 40.0f);
  perlin->addOctave(0.125f, 80.0f);
  perlin->addOctave(0.125f, 160.0f);
  //perlin->addOctave(0.125f, 320.0f);
  //perlin->addOctave(0.125f, 160.0f);
}

LightHit PerlinSpotLight::getLightHit(const Point& p) const
{
  return { (pos - p).normalize(), (pos - p).length() };
}

RGBColor PerlinSpotLight::getIntensity(const LightHit& irr) const
{
  const float dott = dot(dir, -irr.direction.normalize());

  if(dott < cosphi)
    return RGBColor::rep(0.f);

  return intensity * (std::pow(std::abs(dott), exp)/sqr(irr.distance))* perlin->getColor(Point(0,0,0)-irr.direction);
}
}
