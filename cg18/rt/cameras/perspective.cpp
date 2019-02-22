#include <rt/cameras/perspective.h>
#include <rt/ray.h>

#include <cmath>

#include <iostream>

namespace rt
{

PerspectiveCamera::PerspectiveCamera(const Point& center, const Vector& forward,
  const Vector& up, float verticalOpeningAngle, float horizontalOpeningAngle)
  : center(center)
  , focal(forward.normalize())
  , right(
      cross(forward, up).normalize() * std::tan(horizontalOpeningAngle / 2.f))
  , sup(
      cross(right, forward).normalize() * std::tan(verticalOpeningAngle / 2.f))
{
}

Ray PerspectiveCamera::getPrimaryRay(float x, float y) const
{
  return Ray(center, (focal + x * right + y * sup).normalize());
}
}
