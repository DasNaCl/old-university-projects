#pragma once

#include <rt/volumes/volume.h>
#include <vector>

namespace rt
{

class Texture;

class HeterogenousFog : public Volume
{
public:
  HeterogenousFog(Texture* tex, float density);

  RGBColor getColor(const Point& p) override;
  float getTransmittance(float dist) override;
private:
  float density;
  Texture* tex;
};

}

