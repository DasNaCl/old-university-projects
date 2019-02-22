#include <cmath>
#include <rt/volumes/heterogenousfog.h>
#include <rt/textures/texture.h>


namespace rt
{

HeterogenousFog::HeterogenousFog(Texture* tex, float density)
  : Volume()
  , density(density)
  , tex(tex)
{
}

RGBColor HeterogenousFog::getColor(const Point& p)
{
  return tex->getColor(p);
}

float HeterogenousFog::getTransmittance(float dist)
{
  return std::exp(-dist * density);
}

}
