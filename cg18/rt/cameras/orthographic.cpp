#include <rt/cameras/orthographic.h>
#include <rt/ray.h>

namespace rt
{

OrthographicCamera::OrthographicCamera(const Point& center,
  const Vector& forward, const Vector& up, float scaleX, float scaleY)
  : center(center)
  , forward(forward)
  , right(cross(forward, up).normalize())
  , sup(cross(right, forward).normalize())
  , scaleX(scaleX / 2.f)
  , scaleY(scaleY / 2.f)
{
}

Ray OrthographicCamera::getPrimaryRay(float x, float y) const
{
  Point p(center + scaleX * x * right + scaleY * y * sup);

  return Ray(p, forward);
}
}
