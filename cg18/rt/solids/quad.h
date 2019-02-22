#ifndef CG1RAYTRACER_SOLIDS_QUAD_HEADER
#define CG1RAYTRACER_SOLIDS_QUAD_HEADER

#include <core/point.h>
#include <rt/solids/solid.h>
#include <rt/solids/triangle.h>

namespace rt
{

class Quad : public Solid
{
public:
  Quad() {}
  Quad(const Point& v1, const Vector& span1, const Vector& span2,
    CoordMapper* texMapper, Material* material);

  virtual BBox         getBounds() const;
  virtual Intersection intersect(
    const Ray& ray, float previousBestDistance = FLT_MAX) const;
  virtual Sample sample() const;
  virtual float getArea() const;

  Intersection localIntersect(Intersection& inter, const Ray& r) const;

private:
  Point  v1;
  Vector span1;
  Vector span2;

  Triangle tri1, tri2;
};
}

#endif
