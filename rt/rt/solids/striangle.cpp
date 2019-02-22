#include <core/point.h>
#include <rt/intersection.h>
#include <rt/solids/striangle.h>

namespace rt
{

SmoothTriangle::SmoothTriangle(Point vertices[3], Vector normals[3],
  CoordMapper* texMapper, Material* material)
  : Triangle(vertices, texMapper, material)
  , normals({ normals[0], normals[1], normals[2] })
{
}

SmoothTriangle::SmoothTriangle(const Point& v1, const Point& v2,
  const Point& v3, const Vector& n1, const Vector& n2, const Vector& n3,
  CoordMapper* texMapper, Material* material)
  : Triangle(v1, v2, v3, texMapper, material)
  , normals({ n1, n2, n3 })
{
}


Intersection SmoothTriangle::intersect(
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

  const Point bary = { u1.length() / area, u0.length() / area,
    u2.length() / area };
  return Intersection(t, ray, this,
    Vector(normals[0] * bary.x + normals[1] * bary.y + normals[2] * bary.z)
      .normalize(),
    bary);
}
}
