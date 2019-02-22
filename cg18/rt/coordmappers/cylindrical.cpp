#include <core/vector.h>
#include <rt/coordmappers/cylindrical.h>
#include <rt/intersection.h>

namespace rt
{
CylindricalCoordMapper::CylindricalCoordMapper(
  const Point& origin, const Vector& longitudinalAxis, const Vector& polarAxis)
  : center(origin)
  , xscale(polarAxis.length())
  , yscale(0.f)
{
  basis[2] = longitudinalAxis.normalize();
  Vector vtemp(1.f, 0.f, 0.f);
  if(std::abs(basis[2].x - 1.f) <= rt::epsilon)
    vtemp = Vector(0.f, 1.f, 0.f);
  basis[1] = cross(basis[2], vtemp).normalize();
  basis[0] = cross(basis[1], basis[2]).normalize();

  yscale = Vector(dot(longitudinalAxis, basis[0]),
    dot(longitudinalAxis, basis[1]), dot(longitudinalAxis, basis[2]))
             .length();
}
Point CylindricalCoordMapper::getCoords(const Intersection& hit) const
{
  const auto hpv = hit.local() - center;
  const auto loc =
    Vector(dot(hpv, basis[0]), dot(hpv, basis[1]), dot(hpv, basis[2]));
  const float phi =
    (std::atan2(loc.y, loc.x) + 3.141592653f) / (2.f * 3.141592653f);

  return Point(phi / xscale, loc.z / yscale, 0.f);
}
}
