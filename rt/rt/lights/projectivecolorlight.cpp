
#include <core/interpolate.h>
#include <core/random.h>
#include <core/scalar.h>
#include <rt/lights/projectivecolorlight.h>

namespace rt
{

Vector fun(Point& c, Vector p)
{
  int i = 0;
  for(i = 0; i < 512; ++i)
  {
    if(p.lensqr() > 1.0e+8)
      break;
    Vector q = p;
    q.y      = -p.x ;
    q.z      = -p.y ;
    p        = Vector(dot(q, p), cross(q, p).z, 0.0f) + c - Point::rep(0.0f);
  }
  return p.normalize();
}

LightHit ProjectiveColorLight::getLightHit(const Point& p) const
{
  return { (pos - p).normalize(), (pos - p).length() };
}

RGBColor ProjectiveColorLight::getIntensity(const LightHit& irr) const
{
  Vector p = direction;
  Point  c = irr.direction + Point(0, 0, 0);


  const Vector p000 = fun(c, p);
  const auto   p100 = fun(c, Vector(p.x + 1, p.y, p.z));
  const auto   p010 = fun(c, { p.x, p.y + 1, p.z });
  const auto   p001 = fun(c, { p.x, p.y, p.z + 1 });
  const auto   p110 = fun(c, { p.x + 1, p.y + 1, p.z });
  const auto   p111 = fun(c, { p.x + 1, p.y + 1, p.z + 1 });
  const auto   p101 = fun(c, { p.x + 1, p.y, p.z + 1 });
  const auto   p011 = fun(c, { p.x, p.y + 1, p.z + 1 });
  auto         w1   = p000.x - p100.x;
  auto         w2   = p000.y - p010.y;
  auto         w3   = p000.z - p001.z;
  const auto   res =
    lerp3d(p100, p001, p010, p110, p001, p101, p011, p111, w1, w2, w3);
  //std::cout << res.x << " " << res.y << " " << res.z << std::endl;
  return 2 * RGBColor(res.x, res.y, res.z);
}
}
