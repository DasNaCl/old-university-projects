#include <rt/coordmappers/tmapper.h>
#include <rt/intersection.h>
#include <rt/primmod/bmap.h>
#include <rt/solids/triangle.h>
#include <rt/solids/infiniteplane.h>
#include <rt/textures/texture.h>
#include <core/color.h>
#include <core/point.h>
#include <rt/bbox.h>
#include <rt/primitive.h>

  namespace rt
{

  PlaneBumpMapper::PlaneBumpMapper(
    InfinitePlane * base, Texture * bumpmap, float vscale)
    : base(base)
    , bumpmap(bumpmap)
    , scale(scale)
  {
  }

  BBox PlaneBumpMapper::getBounds() const
  {
    return BBox::full();
  }

  Intersection PlaneBumpMapper::intersect(
    const Ray& ray, float previousBestDistance) const
  {
    Intersection inter = base->intersect(ray, previousBestDistance);
    if(!inter)
      return Intersection::failure();

    const Point bumpPoint = inter.local() * 0.01f;
    const Point texPoint(bumpPoint.x, bumpPoint.z, bumpPoint.y);

	Vector bm(bumpmap->getColorDX(texPoint).r, 0, bumpmap->getColorDY(texPoint).r);

    inter.updateNormal(
      (inter.normal() + 1*bm));

    return inter;
  }

  void PlaneBumpMapper::setMaterial(Material * m)
  {
    base->setMaterial(m);
  }

  void PlaneBumpMapper::setCoordMapper(CoordMapper * cm)
  {
    base->setCoordMapper(cm);
  }


  BumpMapper::BumpMapper(Triangle * base, Texture * bumpmap, const Point& bv1,
    const Point& bv2, const Point& bv3, float vscale)
    : base(base)
    , bumpmap(bumpmap)
    , corners({ { bv1, bv2, bv3 } })
    , scale(vscale)
  {
  }

  BBox BumpMapper::getBounds() const
  {
    return base->getBounds();
  }

  Intersection BumpMapper::intersect(const Ray& ray, float previousBestDistance)
    const
  {
    Intersection inter = base->intersect(ray, previousBestDistance);
    if(!inter)
      return Intersection::failure();

    const Point  local     = inter.local();
    auto         x         = (local.x * corners[0]) - Point(0, 0, 0);
    auto         y         = (local.y * corners[1]) - Point(0, 0, 0);
    const Point  bumpPoint = x + y + local.z * corners[2];
    Point        texPoint(bumpPoint.x + 1, bumpPoint.y, 0);
    const Vector normal(0, 0, 1);
    //barycentric coordinates of quad
    const float a1 =
      dot(normal, cross(corners[1] - corners[0], corners[2] - corners[0]));
    const float a2 =
      dot(normal, cross(corners[1] - texPoint, corners[2] - texPoint));
    const float a3 =
      dot(normal, cross(corners[2] - texPoint, corners[0] - texPoint));

    const Point  loc(a2 / a1, a3 / a1, 0.f);
    const auto   v0    = base->vertices[0];
    const Vector v1    = base->vertices[1] - Point(0, 0, 0);
    const Vector v2    = base->vertices[2] - Point(0, 0, 0);
    const Point  testX = v0 * loc.x + v1 * loc.y + v2 * ((1.f - loc.x) - loc.y);


    texPoint = Point(bumpPoint.x, bumpPoint.y + 1, 0);
    const float a11 =
      dot(normal, cross(corners[1] - corners[0], corners[2] - corners[0]));
    const float a22 =
      dot(normal, cross(corners[1] - texPoint, corners[2] - texPoint));
    const float a33 =
      dot(normal, cross(corners[2] - texPoint, corners[0] - texPoint));
    const Point loc1(a22 / a11, a33 / a11, 0.f);
    const Point testY =
      v0 * loc1.x + v1 * loc1.y + v2 * ((1.f - loc1.x) - loc1.y);

    const Vector wx = (testX - inter.hitPoint()).normalize();
    const Vector wy = (inter.hitPoint() - testY).normalize();

    inter.updateNormal(
      (inter.normal() + (bumpmap->getColorDY(bumpPoint).r * wy) -
        bumpmap->getColorDX(bumpPoint).r * wx)
        .normalize());
    return inter;
  }

  void BumpMapper::setMaterial(Material * m)
  {
    base->setMaterial(m);
  }

  void BumpMapper::setCoordMapper(CoordMapper * cm)
  {
    base->setCoordMapper(cm);
  }
}
