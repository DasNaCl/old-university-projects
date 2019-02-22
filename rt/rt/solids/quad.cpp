#include <core/assert.h>
#include <core/random.h>
#include <rt/bbox.h>
#include <rt/intersection.h>
#include <rt/solids/quad.h>
#include <rt/solids/triangle.h>

namespace rt
{

Quad::Quad(const Point& v1, const Vector& span1, const Vector& span2,
  CoordMapper* texMapper, Material* material)
  : Solid(texMapper, material)
  , v1(v1)
  , span1(span1)
  , span2(span2)
{
  Point a = v1 + span1;
  Point b = v1 + span2;
  Point c = v1 + span1 + span2;

  Point t1[]{ v1, a, b };
  tri1 = Triangle(t1, texMapper, material);
  Point t2[]{ a, c, b };
  tri2 = Triangle(t2, texMapper, material);
}

BBox Quad::getBounds() const
{
  BBox b = BBox::empty();

  b.extend(tri1.getBounds());
  b.extend(tri2.getBounds());

  return b;
}

Intersection Quad::intersect(const Ray& ray, float previousBestDistance) const
{
  Intersection inter    = tri1.intersect(ray, previousBestDistance);
  Intersection finalInt = localIntersect(inter, ray);
  if(finalInt)
  {
    return finalInt;
  }
  inter    = tri2.intersect(ray, previousBestDistance);
  finalInt = localIntersect(inter, ray);
  return finalInt;
}

Intersection Quad::localIntersect(Intersection& inter, const Ray& r) const
{
  if(inter)
  {
    const Point hit = inter.hitPoint();
    float       u   = dot((hit - v1), (span1));
    float       v   = dot((hit - v1), (span2));
    return { inter.distance, r, this, cross(span1, span2),
        { u / span1.lensqr(), v / span2.lensqr(), 0 } };
  }
  return Intersection::failure();
}
Solid::Sample Quad::sample() const
{
  const float rnd0 = rt::random(0.f, 1.f);
  const float rnd1 = rt::random(0.f, 1.f);

  const auto point = (v1 + rnd0 * span2 + rnd1 * span1) - Vector(0, epsilon, 0);
  return { point, cross(span1, span2).normalize() };
}

float Quad::getArea() const
{
  return cross(span1, span2).length();
}
}
