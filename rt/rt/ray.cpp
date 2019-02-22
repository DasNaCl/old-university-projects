#include <rt/ray.h>

namespace rt
{

Ray::Ray(const Point& o, const Vector& d)
  : o(o)
  , d(d)
{
}

Point Ray::getPoint(float dist) const
{
  return o + d * dist;
}
}
