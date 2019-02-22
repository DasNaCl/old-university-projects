#include <core/assert.h>
#include <core/random.h>
#include <rt/bbox.h>
#include <rt/intersection.h>
#include <rt/solids/sphere.h>
#include <cmath>

namespace rt
{

Sphere::Sphere(
  const Point& center, float radius, CoordMapper* texMapper, Material* material)
  : Solid(texMapper, material)
  , center(center)
  , radius(radius)
{
}

BBox Sphere::getBounds() const
{
  assert(radius > 0);
  return BBox(center - Vector::rep(radius), center + Vector::rep(radius));
}

Intersection Sphere::intersect(const Ray& ray, float previousBestDistance) const
{
  Vector l  = ray.d.normalize();
  Vector oc = ray.o - center;

  float dot_l_oc = dot(l, oc);
  float t        = sqr(dot_l_oc) - oc.lensqr() + sqr(radius);

  //not sure what exactly happens when the ray hits in exactly one point
  if(t >= 0)
  {
    float x1 = -dot_l_oc - sqrt(t);
    float x2 = -dot_l_oc + sqrt(t);
    auto hit = ray.getPoint(std::min(x1, x2))-center;
    float r = sqrt(sqr(hit.x)+sqr(hit.y)+sqr(hit.z));
    float theta = atan(hit.x/hit.y);
    float phi = acos(hit.z/r);
    //Point local(radius,abs(dot(hit,{1,0,0})),abs(dot(hit,{0,0,1})));
    Point local(r,theta,phi);
    if(std::min(x1, x2) > 0 && std::min(x1, x2) < previousBestDistance)
    {
      Vector normal = (ray.getPoint(std::min(x1, x2)) - center);
      return Intersection(std::min(x1, x2), ray, this, normal.normalize(), ray.getPoint(std::min(x1, x2))/*(Point(0,0,0) + normal)*(1/(2*radius))*/);
    }
    else if(std::max(x1, x2) > 0 && std::max(x1, x2) < previousBestDistance)
    {
      Vector normal = (ray.getPoint(std::max(x1, x2)) - center);
        return Intersection(std::max(x1, x2), ray, this, normal.normalize(), ray.getPoint(std::min(x1, x2)/*(Point(0,0,0) + normal)*(1/(2*radius))*/));
    }
  }
  return Intersection::failure();
}

Solid::Sample Sphere::sample() const
{
redo:
  const float rnd0 = rt::random(0.f, 1.f);
  const float rnd1 = rt::random(0.f, 1.f);
  if(sqr(rnd0) + sqr(rnd1) >= 1.f)
    goto redo;

  const float tmp = std::sqrt(1.f - sqr(rnd0) - sqr(rnd1));
  const float x = 2.f * rnd0 * tmp;
  const float y = 2.f * rnd1 * tmp;
  const float z = 1.f - 2.f * (sqr(rnd0) + sqr(rnd1));
  const Point P(radius * x, radius * y, radius * z);
  return { P, (P - center).normalize() };
}

float Sphere::getArea() const
{
  return 4 * pi * sqr(radius);
}
}
