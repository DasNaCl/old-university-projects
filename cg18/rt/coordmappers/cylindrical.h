#ifndef CG1RAYTRACER_COORDMAPPERS_CYLINDRICAL_HEADER
#define CG1RAYTRACER_COORDMAPPERS_CYLINDRICAL_HEADER

#include <array>
#include <core/point.h>
#include <core/vector.h>
#include <rt/coordmappers/coordmapper.h>

namespace rt
{

class Intersection;

class CylindricalCoordMapper : public CoordMapper
{
public:
  CylindricalCoordMapper(const Point& origin, const Vector& longitudinalAxis,
    const Vector& polarAxis);
  virtual Point getCoords(const Intersection& hit) const;

private:
  Point                 center;
  float                 xscale;
  float                 yscale;
  std::array<Vector, 3> basis;
};

}

#endif
