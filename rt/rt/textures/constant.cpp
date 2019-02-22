//
// Created by Matthias on 13.12.2018.
//

#include <core/assert.h>
#include <rt/textures/constant.h>

namespace rt
{
ConstantTexture::ConstantTexture()
{
}

ConstantTexture::ConstantTexture(const RGBColor& color)
  : color(color)
{
}

RGBColor ConstantTexture::getColor(const Point& coord)
{
  return color;
}

RGBColor ConstantTexture::getColorDX(const Point& coord)
{
  return color;
}

RGBColor ConstantTexture::getColorDY(const Point& coord)
{
  return color;
}
}