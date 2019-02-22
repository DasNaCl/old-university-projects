#pragma once

#include <core/matrix.h>
#include <rt/primitive.h>

namespace rt
{

/**
 * This motion primmod can give motion to any primitive.
 * It needs a const ref to the renderer to get the current time
 */

class Renderer;

class Motion : public Primitive
{
public:
  Motion(Primitive* content, Renderer& renderer);

  Primitive* content();

  void reset();
  void setMoveRate(const Vector& acceleration,
    const Vector&
      velocity0);  // <- TODO: allow for different translation functions, current implementation is just uniform accelerated movement

  BBox         getBounds() const override;
  Intersection intersect(
    const Ray& ray, float previousBestDistance = FLT_MAX) const override;
  void setMaterial(Material* m) override;
  void setCoordMapper(CoordMapper* cm) override;

private:
  Vector acceleration;
  Vector velocity0;

  Primitive* prim;
  Renderer&  renderer;
};
}
