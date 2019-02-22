#pragma once

#include <rt/volumes/volume.h>

namespace rt
{

class UniformFogAmbientLight : public Volume
{
public:
  UniformFogAmbientLight(RGBColor ambientColor, float density);

  RGBColor getColor(const Point& p) override;
  float getTransmittance(float dist) override;

  bool isLitByAmbient() const override
  {
    return true;
  }

private:
  RGBColor ambientColor;
  float density;
};

}

