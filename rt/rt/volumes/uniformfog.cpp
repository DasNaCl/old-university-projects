#include <cmath>
#include <rt/volumes/uniformfog.h>

namespace rt
{

UniformFog::UniformFog(RGBColor col, float density)
  : Volume()
  , density(density)
  , col(col)
{
}

RGBColor UniformFog::getColor(const Point& p)
{
  return col;
}

float UniformFog::getTransmittance(float dist)
{
  return std::exp(-dist * density);
}
}
