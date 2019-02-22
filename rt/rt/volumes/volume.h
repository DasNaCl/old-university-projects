#pragma once

#include <core/color.h>

namespace rt
{

class Point;
class Primitive;

class Volume
{
public:
  virtual RGBColor getColor(const Point& p) = 0;
  virtual float getTransmittance(float dist) = 0;

  virtual bool isLitByAmbient() const
  {
    return false;
  }
  virtual bool isHeterogenous() const
  {
    return false;
  }
};

}

