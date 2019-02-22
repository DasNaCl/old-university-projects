#include <core/assert.h>
#include <numeric>
#include <rt/intersection.h>
#include <rt/solids/aabox.h>

namespace rt
{

AABox::AABox(const Point& corner1, const Point& corner2, CoordMapper* texMapper,
  Material* material)
  : Solid(texMapper, material)
{
  Vector v1 = corner1 - Point(0, 0, 0);
  Vector v2 = corner2 - Point(0, 0, 0);
  upper =
    rt::max(corner1, corner2);  //v1.length() < v2.length() ? corner2 : corner1;
  lower =
    rt::min(corner1, corner1);  //v1.length() < v2.length() ? corner1 : corner2;

  float x = std::abs(upper.x - lower.x);
  float y = std::abs(upper.y - lower.y);
  float z = std::abs(upper.z - lower.z);

  sides[0] =
    Quad(lower, Vector(0, 1, 0) * y, Vector(1, 0, 0) * x, texMapper, material);
  sides[1] =
    Quad(lower, Vector(1, 0, 0) * x, Vector(0, 0, 1) * z, texMapper, material);
  sides[2] =
    Quad(lower, Vector(0, 0, 1) * z, Vector(0, 1, 0) * y, texMapper, material);
  sides[3] = Quad(
    upper,  Vector(-1, 0, 0) * x, Vector(0, -1, 0) * y, texMapper, material);
  sides[4] = Quad(
    upper, Vector(0, 0, -1) * z, Vector(-1, 0, 0) * x, texMapper, material);
  sides[5] = Quad(
    upper,  Vector(0, -1, 0) * y, Vector(0, 0, -1) * z, texMapper, material);
}

BBox AABox::getBounds() const
{
  return BBox(lower, upper);
}

Intersection AABox::intersect(const Ray& ray, float previousBestDistance) const
{
  return std::accumulate(sides.begin(), sides.end(), Intersection::failure(),
    [&ray, &previousBestDistance](
      const Intersection& currentInter, const Quad& obj) {
      Intersection intermediateInter = obj.intersect(ray, previousBestDistance);
      if(0.f < intermediateInter.distance &&
         intermediateInter.distance < previousBestDistance)
      {
        previousBestDistance = intermediateInter.distance;
        return intermediateInter;
      }
      return currentInter;
    });
}

Solid::Sample AABox::sample() const
{
  NOT_IMPLEMENTED;
}

float AABox::getArea() const
{
  return 2 * (sides[0].getArea() + sides[1].getArea() + sides[2].getArea());
}
}
