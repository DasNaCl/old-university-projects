#pragma once

#include <array>
#include <rt/coordmappers/coordmapper.h>

namespace rt
{

class Intersection;
class EnvironmnetMapper : public CoordMapper
{
public:
  EnvironmnetMapper(const Vector& zenith, const Vector& azimuthRef);

  Point getCoords(const Intersection& hit) const override;

private:
  std::array<Vector, 3> basis;
  float                 phi;
  Vector                zenith;
};
}
