#ifndef CG1RAYTRACER_PRIMMOD_BMAP_HEADER
#define CG1RAYTRACER_PRIMMOD_BMAP_HEADER

#include <core/point.h>
#include <rt/primitive.h>
#include <array>

namespace rt
{
class InfinitePlane;
class Triangle;
class Texture;

class PlaneBumpMapper : public Primitive
{
public:
  PlaneBumpMapper(InfinitePlane* base, Texture* bumpmap, float vscale);
  virtual BBox getBounds() const;
  virtual Intersection intersect(
    const Ray& ray, float previousBestDistance = FLT_MAX) const;
  virtual void setMaterial(Material* m);
  virtual void setCoordMapper(CoordMapper* cm);

private:
  InfinitePlane* base;
  Texture* bumpmap;
  float scale;
};

class BumpMapper : public Primitive
{
public:
  BumpMapper(Triangle* base, Texture* bumpmap, const Point& bv1, const Point& bv2, const Point& bv3, float vscale);

  virtual BBox getBounds() const;
  virtual Intersection intersect(const Ray& ray, float previousBestDistance = FLT_MAX) const;
  virtual void setMaterial(Material* m);
  virtual void setCoordMapper(CoordMapper* cm);
private:
  Triangle* base;
  Texture* bumpmap;
  std::array<Point, 3> corners;
  float scale;

};

}

#endif
