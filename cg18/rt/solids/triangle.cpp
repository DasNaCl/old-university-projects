#include <core/assert.h>
#include <core/random.h>
#include <rt/bbox.h>
#include <rt/intersection.h>
#include <rt/solids/infiniteplane.h>
#include <rt/solids/triangle.h>
namespace rt
{

Triangle::Triangle(
  Point vertices[3], CoordMapper* texMapper, Material* material)
  : Solid(texMapper, material)
  , vertices{ vertices[0], vertices[1], vertices[2] }
  , u(cross(vertices[1] - vertices[0], vertices[2] - vertices[0]))
{
}

Triangle::Triangle(const Point& v1, const Point& v2, const Point& v3,
  CoordMapper* texMapper, Material* material)
  : Solid(texMapper, material)
  , vertices{ v1, v2, v3 }
  , u(cross(vertices[1] - vertices[0], vertices[2] - vertices[0]))
{
}

BBox Triangle::getBounds() const
{
  const auto v1 = vertices[0];
  const auto v2 = vertices[1];
  const auto v3 = vertices[2];
  return BBox(Point(std::min(v1.x, std::min(v2.x, v3.x)),
                std::min(v1.y, std::min(v2.y, v3.y)),
                std::min(v1.z, std::min(v2.z, v3.z))),
    Point(std::max(v1.x, std::max(v2.x, v3.x)),
      std::max(v1.y, std::max(v2.y, v3.y)),
      std::max(v1.z, std::max(v2.z, v3.z))));
}

Intersection Triangle::intersect(
  const Ray& ray, float previousBestDistance) const
{
  const auto  normal = u.normalize();
  const float denom  = dot(ray.d, normal);
  if(std::abs(denom) <= rt::epsilon)
    return Intersection::failure();
  const float t = -dot(ray.o - vertices[0], normal) / denom;

  if(t < rt::epsilon || t > previousBestDistance)
    return Intersection::failure();

  const Point surface = ray.getPoint(t);

  const float area = u.length();

  const Vector u0 = cross(surface - vertices[0], vertices[2] - vertices[0]);
  const Vector u1 = cross(vertices[1] - surface, vertices[2] - surface);
  const Vector u2 = cross(vertices[1] - vertices[0], surface - vertices[0]);

  if(dot(u, u0) < 0 || dot(u, u1) < 0 || dot(u, u2) < 0)
    return Intersection::failure();
  return Intersection(t, ray, this, normal,
    {  u1.length() / area ,u0.length() / area,u2.length() / area});
}

Solid::Sample Triangle::sample() const
{
  // See http://www.cs.princeton.edu/~funk/tog02.pdf
  const float rnd0 = std::sqrt(rt::random(0.f, 1.f));
  const float rnd1 = rt::random(0.f, 1.f);

  const Point point = (vertices[0] * (1.f - rnd0) - Point::rep(0.f)) +
                      (vertices[1] * (rnd0 * (1.f - rnd1)) - Point::rep(0.f)) +
                      vertices[2] * (rnd1 * rnd0);
  return { point, u.normalize() };
}

float Triangle::getArea() const
{
  return u.length() / 2;
}
}
