#include <rt/volumes/uniformfogambientlight.h>
#include <core/point.h>
#include <core/vector.h>
#include <cmath>

namespace rt
{

UniformFogAmbientLight::UniformFogAmbientLight(
  RGBColor ambientColor, float density)
  : Volume()
  , ambientColor(ambientColor)
  , density(density)
{

}

RGBColor UniformFogAmbientLight::getColor(const Point& p)
{
  return ambientColor;
}

float UniformFogAmbientLight::getTransmittance(float dist)
{
  return std::exp(-dist * density);
}


}

