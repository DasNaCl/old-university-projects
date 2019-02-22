#include <cmath>
#include <limits>
#include <rt/bbox.h>
#include <rt/ray.h>

namespace rt
{
constexpr static float inf = std::numeric_limits<float>::infinity();

BBox BBox::empty()
{
  return BBox(Point(inf, inf, inf), Point(-inf, -inf, -inf));
}
BBox BBox::full()
{
  return BBox(Point(-inf, -inf, -inf), Point(inf, inf, inf));
}
void BBox::extend(const Point& point)
{
  min.x = std::fmin(min.x, point.x);
  min.y = std::fmin(min.y, point.y);
  min.z = std::fmin(min.z, point.z);

  max.x = std::fmax(max.x, point.x);
  max.y = std::fmax(max.y, point.y);
  max.z = std::fmax(max.z, point.z);
}
void BBox::extend(const BBox& box)
{
  min.x = std::fmin(min.x, box.min.x);
  min.y = std::fmin(min.y, box.min.y);
  min.z = std::fmin(min.z, box.min.z);

  max.x = std::fmax(max.x, box.max.x);
  max.y = std::fmax(max.y, box.max.y);
  max.z = std::fmax(max.z, box.max.z);
}
std::pair<float, float> BBox::intersect(const Ray& ray) const
{
  const Vector inv(1.f / ray.d.x, 1.f / ray.d.y, 1.f / ray.d.z);

  const float t1 = (max.x - ray.o.x) * inv.x;
  const float t2 = (min.x - ray.o.x) * inv.x;
  const float t3 = (max.y - ray.o.y) * inv.y;
  const float t4 = (min.y - ray.o.y) * inv.y;
  const float t5 = (max.z - ray.o.z) * inv.z;
  const float t6 = (min.z - ray.o.z) * inv.z;

  const float tmin = std::fmax(
    std::fmax(std::fmin(t1, t2), std::fmin(t3, t4)), std::fmin(t5, t6));
  const float tmax = std::fmin(
    std::fmin(std::fmax(t1, t2), std::fmax(t3, t4)), std::fmax(t5, t6));

  // first part commented out, as negative values are permitted
  if(/*tmax < 0 ||*/ tmin > tmax)
    return std::make_pair(inf, -inf);
  return std::make_pair(tmin, tmax);
}
bool BBox::isUnbound()
{
  return std::isinf(min.x) || std::isinf(min.y) || std::isinf(min.z) ||
         std::isinf(max.x) || std::isinf(max.y) || std::isinf(max.z);
}
}
