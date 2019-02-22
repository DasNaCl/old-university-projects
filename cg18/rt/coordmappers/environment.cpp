#include <core/point.h>
#include <core/scalar.h>
#include <rt/coordmappers/environment.h>
#include <rt/intersection.h>

namespace rt
{

EnvironmnetMapper::EnvironmnetMapper(
  const Vector& zenith, const Vector& azimuthRef)
  : basis()
  , phi()
  , zenith()
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

  const auto azimuth = Vector(dot(azimuthRef, basis[0]),
    dot(azimuthRef, basis[1]), dot(azimuthRef, basis[2]))
                         .normalize();
  phi = std::atan2(azimuth.y, azimuth.x) + 3.141592653f;
}
Point EnvironmnetMapper::getCoords(const Intersection& hit) const
{
  const auto loc = Vector(dot(hit.ray.d, basis[0]), dot(hit.ray.d, basis[1]),
    dot(hit.ray.d, basis[2]));

  float psi = std::atan2(loc.y, loc.x) + phi + 3.141592653f;
  while(psi > 2.f * 3.141592653f)
    psi -= 2.f * 3.141592654f;
  const float theta = std::acos(dot(loc, zenith));

  return Point(phi / 2.f * 3.141592654f, theta / 3.141592654f, 0.f);
}
}
