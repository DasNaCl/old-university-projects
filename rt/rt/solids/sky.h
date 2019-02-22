#pragma once

#include <rt/solids/solid.h>

namespace rt
{

class Sky : public Solid
{
public:
  Sky() {}
  Sky(CoordMapper* texMapper, Material* material);

  virtual BBox         getBounds() const;
  virtual Intersection intersect(
    const Ray& ray, float previousBestDistance = FLT_MAX) const;
  virtual Sample sample() const;
  virtual float getArea() const;
};
}
