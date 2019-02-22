#pragma once

#include <rt/volumes/volume.h>

namespace rt
{

class UniformFog : public Volume
{
public:
  UniformFog(RGBColor col, float density);

  RGBColor getColor(const Point& p) override;
  float    getTransmittance(float dist) override;

private:
  float    density;
  RGBColor col;
};
}
