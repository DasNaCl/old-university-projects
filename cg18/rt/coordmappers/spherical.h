#ifndef CG1RAYTRACER_COORDMAPPERS_SPHERICAL_HEADER
#define CG1RAYTRACER_COORDMAPPERS_SPHERICAL_HEADER

#include <array>
#include <core/point.h>
#include <core/vector.h>
#include <rt/coordmappers/coordmapper.h>

namespace rt
{

class Intersection;
class SphericalCoordMapper : public CoordMapper
{
public:
  SphericalCoordMapper();
  SphericalCoordMapper(
    const Point& origin, const Vector& zenith, const Vector& azimuthRef);
  virtual Point getCoords(const Intersection& hit) const;

private:
  Point                 center;
  float                 xscale;
  float                 yscale;
  std::array<Vector, 3> basis;
  Vector                zenith;
};

}

#endif
