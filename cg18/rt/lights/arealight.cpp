#include <core/color.h>
#include <core/point.h>
#include <rt/lights/arealight.h>
#include <rt/materials/material.h>
#include <rt/solids/solid.h>

namespace rt {

LightHit AreaLight::getLightHit(const Point& p) const {

    const auto sample = source->sample();
    const auto tmp    = sample.point - p;

    return { tmp.normalize(), tmp.length(), sample.normal };
}


RGBColor AreaLight::getIntensity(const LightHit& irr) const {
  const auto intensity =
    source->material->getEmission({ 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 });
  return (intensity * source->getArea() * (dot(irr.normal, -irr.direction))) /
         sqr(irr.distance);
}

AreaLight::AreaLight( Solid* source ):
source(source)
{
}

}
