#include <cmath>
#include <core/random.h>
#include <rt/cameras/dofperspective.h>
#include <rt/ray.h>

namespace rt
{

DOFPerspectiveCamera::DOFPerspectiveCamera(const Point& center,
  const Vector& forward, const Vector& up, float verticalOpeningAngle,
  float horizontalOpeningAngle, float focalDistance, float apertureRadius)
  : center(center)
  , focal(forward)
  , right(
      cross(forward, up).normalize() * std::tan(horizontalOpeningAngle / 2.f))
  , sup(
      cross(right, forward).normalize() * std::tan(verticalOpeningAngle / 2.f))
  , ratio(verticalOpeningAngle / horizontalOpeningAngle)
  , angle(ratio > 1.f ? verticalOpeningAngle : horizontalOpeningAngle)
  , focus(1.f / std::tan(angle / 2.f))
  , focalDist(focalDistance)
  , apertureRadius(apertureRadius)
{
}

Ray DOFPerspectiveCamera::getPrimaryRay(float x, float y) const
{
  Ray r = { center, (focal + x * right + y * sup).normalize() };//TODO normal

  if(apertureRadius > 0.f)
  {
    const float r0 = random(0.f, 1.f);
    const float r1 = random(0.f, 1.f);

    const float radius = std::sqrt(r0) * std::sqrt(apertureRadius);
    const float theta  = 2.f * 3.141592653f * r1;

    const Point cart(radius * std::cos(theta), radius * std::sin(theta), 0.f);

    r.o = center + cart.x * right + cart.y * sup;
    r.d = ((focalDist * r.d) - (r.o - center)).normalize();
  }
  return r;
}
}
