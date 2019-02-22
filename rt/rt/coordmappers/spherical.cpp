#include <core/vector.h>
#include <rt/coordmappers/spherical.h>
#include <rt/intersection.h>

namespace rt
{
SphericalCoordMapper::SphericalCoordMapper() {}
SphericalCoordMapper::SphericalCoordMapper(
  const Point& origin, const Vector& zenith, const Vector& azimuthRef)
  : center(origin)
  , xscale(azimuthRef.length())
  , yscale(zenith.length())
  , zenith(zenith)
{
  basis[2] = zenith.normalize();
  Vector vtemp(1.f, 0.f, 0.f);
  if(std::abs(basis[2].x - 1.f) <= rt::epsilon)
    vtemp = Vector(0.f, 1.f, 0.f);
  basis[1] = cross(basis[2], vtemp).normalize();
  basis[0] = cross(basis[1], basis[2]).normalize();

  this->zenith =
    Vector(dot(zenith, basis[0]), dot(zenith, basis[1]), dot(zenith, basis[2]))
      .normalize();
}
Point SphericalCoordMapper::SphericalCoordMapper::getCoords(
  const Intersection& hit) const
{
  const auto p   = hit.hitPoint() - center;
  const auto loc = Vector(dot(p, basis[0]), dot(p, basis[1]), dot(p, basis[2]));

  const auto proj = Vector(loc.x, loc.y, 0.f).normalize();

  const float phi =
    (std::atan2(proj.y, proj.x) + 3.141592653f) / (2.f * 3.141592653f);
  const float theta = std::acos(dot(loc.normalize(), zenith)) / 3.141592653f;

  return Point(phi / xscale, theta / yscale, 0.f);
}
}
