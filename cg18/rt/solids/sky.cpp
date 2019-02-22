#include <rt/bbox.h>
#include <rt/intersection.h>
#include <rt/solids/sky.h>

namespace rt
{

Sky::Sky(CoordMapper* texMapper, Material* material)
  : Solid(texMapper, material)
{
}
BBox Sky::getBounds() const
{
  return BBox::full();
}
Intersection Sky::intersect(const Ray& ray, float previousBestDistance) const
{
  if(previousBestDistance < std::numeric_limits<float>::infinity() &&
     previousBestDistance != FLT_MAX)
    return Intersection::failure();
  return Intersection(std::numeric_limits<float>::infinity(), ray, this, ray.d,
    Point::rep(std::numeric_limits<float>::infinity()));
}
Solid::Sample Sky::sample() const
{
  const auto inv = Point::rep(std::numeric_limits<float>::infinity());

  return { inv, inv - Point::rep(0.f) };
}
float Sky::getArea() const
{
  return std::numeric_limits<float>::infinity();
}
}
