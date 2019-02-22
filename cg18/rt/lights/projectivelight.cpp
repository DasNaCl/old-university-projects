#include "projectivelight.h"
#include <core/julia.h>
#include <main/a1.h>

namespace rt
{

ProjectiveLight::ProjectiveLight(
  const Point& position, const float width, const float heigth)
  : w(width)
  , h(heigth)
  , pos(position)
{
}
LightHit ProjectiveLight::getLightHit(const Point& p) const
{
  return { (pos - p).normalize(), (pos - p).length() };
}

RGBColor ProjectiveLight::getIntensity(const LightHit& irr) const
{
  return a1computeColor(irr.direction.x, irr.direction.y, w, h);
}
}
