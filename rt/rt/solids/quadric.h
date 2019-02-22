#ifndef CG1RAYTRACER_SOLIDS_QUADRIC_HEADER
#define CG1RAYTRACER_SOLIDS_QUADRIC_HEADER

#include <array>
#include <core/assert.h>
#include <rt/bbox.h>
#include <rt/solids/solid.h>

namespace rt
{

class Quadric : public Solid
{
public:
  struct Coefficients
  {
    float a{ 0.f };  // x^2
    float b{ 0.f };  // y^2
    float c{ 0.f };  // z^2
    float d{ 0.f };  // xy
    float e{ 0.f };  // xz
    float f{ 0.f };  // yz
    float g{ 0.f };  // x
    float h{ 0.f };  // y
    float i{ 0.f };  // z
    float j{ 0.f };  //

    static Coefficients ellipsoid(
      float x0, float y0, float z0, float r1, float r2, float r3)
    {
      //     ((x - x0) / r1)^2 + ((y - y0) / r2)^2 + ((z - z0) / r3)^2 = 1

      Coefficients cof;

      cof.a = sqr(r2) * sqr(r3);
      cof.b = sqr(r1) * sqr(r3);
      cof.c = sqr(r1) * sqr(r2);
      cof.g = -2.f * sqr(r2) * sqr(r3) * x0;
      cof.h = -2.f * sqr(r1) * sqr(r3) * y0;
      cof.i = -2.f * sqr(r1) * sqr(r2) * z0;
      cof.j = sqr(r2) * sqr(r3) * sqr(x0) + sqr(r1) * sqr(r3) * sqr(y0) +
              sqr(r1) * sqr(r2) * sqr(z0) - sqr(r1) * sqr(r2) * sqr(r3);

      return cof;
    }

    static Coefficients paraboloid(
      float x0, float y0, float z0, float r1, float r2)
    {
      //    ((x - x0) / r1)^2 + ((y - y0) / r2)^2 - z = 0

      Coefficients cof;

      cof.a = sqr(r2);
      cof.b = sqr(r1);
      cof.g = -2.f * sqr(r2) * x0;
      cof.h = -2.f * sqr(r1) * y0;
      cof.i = -1.f * sqr(r1) * sqr(r2);
      cof.j = sqr(r2) * sqr(x0) + sqr(r1) * sqr(y0) + sqr(r1) * sqr(r2) * z0;

      return cof;
    }

    static Coefficients cylinder(float x0, float y0, float r1, float r2)
    {
      //     ((x - x0) / r1)^2 + ((y - y0) / r2)^2 = 1

      Coefficients cof;

      cof.a = sqr(r2);
      cof.b = sqr(r1);
      cof.g = -2.f * sqr(r2) * x0;
      cof.h = -2.f * sqr(r1) * y0;
      cof.j = sqr(r2) * sqr(x0) + sqr(r1) * sqr(y0) - sqr(r1) * sqr(r2);

      return cof;
    }

    Vector normalAt(Point p) const
    {
      return Vector(2 * a * p.x + d * p.y + e * p.z + g,
        2 * b * p.y + d * p.x + f * p.z + h,
        2 * c * p.z + e * p.x + f * p.y + i)
        .normalize();
    }

    float operator[](std::size_t i) const
    {
      if /*constexpr*/ (i == 0)
        return a;
      else if(i == 1)
        return b;
      else if(i == 2)
        return c;
      else if(i == 3)
        return d;
      else if(i == 4)
        return e;
      else if(i == 5)
        return f;
      else if(i == 6)
        return g;
      else if(i == 7)
        return h;
      else if(i == 8)
        return this->i;
      else if(i == 9)
        return j;

      UNREACHABLE;
    }
    float& operator[](std::size_t i)
    {
      if /*constexpr*/ (i == 0)
        return a;
      else if(i == 1)
        return b;
      else if(i == 2)
        return c;
      else if(i == 3)
        return d;
      else if(i == 4)
        return e;
      else if(i == 5)
        return f;
      else if(i == 6)
        return g;
      else if(i == 7)
        return h;
      else if(i == 8)
        return this->i;
      else if(i == 9)
        return j;

      UNREACHABLE;
    }
  };

public:
  Quadric() {}
  Quadric(const Coefficients& cof, CoordMapper* texMapper, Material* material);

  virtual BBox         getBounds() const;
  virtual Intersection intersect(
    const Ray& ray, float previousBestDistance = FLT_MAX) const;
  virtual Sample sample() const;
  virtual float getArea() const;

private:
  Coefficients cof;
};
}

#endif
