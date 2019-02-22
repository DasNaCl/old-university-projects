#include <core/vector.h>
#include <rt/coordmappers/tmapper.h>
#include <rt/intersection.h>

namespace rt
{
TriangleMapper::TriangleMapper(
  const Point& tv0, const Point& tv1, const Point& tv2)
  : points({ Float4(tv0), Float4(tv1), Float4(tv2) })
{
}
TriangleMapper::TriangleMapper(Point ntv[3])
  : points({ Float4(ntv[0]), Float4(ntv[1]), Float4(ntv[2]) })
{
}
Point TriangleMapper::getCoords(const Intersection& hit) const
{
  const auto res = hit.local().x * points[0] + hit.local().y * points[1] +
                   hit.local().z * points[2];
  return Point(res[0], res[1], res[2]);
}
}
