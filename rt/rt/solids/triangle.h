#ifndef CG1RAYTRACER_SOLIDS_TRIANGLE_HEADER
#define CG1RAYTRACER_SOLIDS_TRIANGLE_HEADER

#include <array>
#include <core/point.h>
#include <rt/solids/solid.h>

namespace rt
{

class Triangle : public Solid
{
public:
  Triangle() {}
  Triangle(Point vertices[3], CoordMapper* texMapper, Material* material);
  Triangle(const Point& v1, const Point& v2, const Point& v3,
    CoordMapper* texMapper, Material* material);

  virtual BBox         getBounds() const;
  virtual Intersection intersect(
    const Ray& ray, float previousBestDistance = FLT_MAX) const;
  virtual Sample sample() const;
  virtual float getArea() const;

  const Point& operator[](std::size_t pos) { return vertices[pos]; }

  std::array<Point, 3> vertices;
protected:
  Vector               u;
};
}

#endif


