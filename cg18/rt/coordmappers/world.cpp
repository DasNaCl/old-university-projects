#include <core/vector.h>
#include <rt/coordmappers/world.h>
#include <rt/intersection.h>

namespace rt
{
WorldMapper::WorldMapper()
  : m(Matrix::identity())
{
}
WorldMapper::WorldMapper(const Float4& scale)
  : m({ { scale[0], 0.f, 0.f, 0.f }, { 0.f, scale[1], 0.f, 0.f },
      { 0.f, 0.f, scale[2], 0.f }, { 0.f, 0.f, 0.f, scale[3] } })
{
}
Point WorldMapper::getCoords(const Intersection& hit) const
{
  const auto tmp = m * Float4(hit.hitPoint());
  return Point(tmp[0], tmp[1], tmp[2]);
}
}
