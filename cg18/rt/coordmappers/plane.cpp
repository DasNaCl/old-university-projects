#include <core/float4.h>
#include <rt/coordmappers/plane.h>
#include <rt/intersection.h>

namespace rt
{
PlaneCoordMapper::PlaneCoordMapper(const Vector& e1, const Vector& e2)
  : m(Matrix::inverseOf(Float4(e1.x, e2.x, (cross(e1, e2)).x, 0),
      Float4(e1.y, e2.y, (cross(e1, e2)).y, 0),
      Float4(e1.z, e2.z, (cross(e1, e2)).z, 0), Float4(0, 0, 0, 1)))
{
}
Point PlaneCoordMapper::getCoords(const Intersection& hit) const
{
  const auto tmp = m * Float4(hit.local());
  return Point(tmp[0], tmp[1], 0.f);
}
}
