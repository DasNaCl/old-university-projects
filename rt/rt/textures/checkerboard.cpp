//
// Created by Matthias on 13.12.2018.
//

#include <core/assert.h>
#include <core/point.h>
#include <rt/textures/checkerboard.h>
#include <math.h>
namespace rt
{
CheckerboardTexture::CheckerboardTexture(
  const RGBColor& white, const RGBColor& black)
  : white(white)
  , black(black)
{
}
RGBColor CheckerboardTexture::getColor(const Point& coord)
{
  float      x =  floorf(2.f * coord.x);
  float      y =  floorf(2.f * coord.y);
  float      z =  floorf(2.f * coord.z);
  const auto co0    = int(x) % 2 == 0;
  const auto co1    = int(y) % 2 == 0;
  const auto co2    = int(z) % 2 == 0;

  return (co0 ^ co1 ^ co2) ? white : black;
}
RGBColor CheckerboardTexture::getColorDX(const Point& coord)
{
  NOT_IMPLEMENTED;
}
RGBColor CheckerboardTexture::getColorDY(const Point& coord)
{
  NOT_IMPLEMENTED;
}
}