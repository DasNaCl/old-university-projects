#ifndef CG1RAYTRACER_SOLIDS_BOX_HEADER
#define CG1RAYTRACER_SOLIDS_BOX_HEADER

#include <array>
#include <rt/bbox.h>
#include <rt/solids/quad.h>
#include <rt/solids/solid.h>

namespace rt
{

class AABox : public Solid
{
public:
  AABox() {}
  AABox(const Point& corner1, const Point& corner2, CoordMapper* texMapper,
    Material* material);

  virtual BBox         getBounds() const;
  virtual Intersection intersect(
    const Ray& ray, float previousBestDistance = FLT_MAX) const;
  virtual Sample sample() const;
  virtual float getArea() const;

private:
  Point lower;
  Point upper;
  std::array<Quad, 6> sides;
};
}

#endif
