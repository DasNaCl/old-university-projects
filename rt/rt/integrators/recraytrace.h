#ifndef CG1RAYTRACER_INTEGRATORS_RECURSIVERAYTRACING_HEADER
#define CG1RAYTRACER_INTEGRATORS_RECURSIVERAYTRACING_HEADER

#include <rt/integrators/integrator.h>
#include <cstddef>

namespace rt
{

class World;
class Ray;
class RGBColor;

class RecursiveRayTracingIntegrator : public Integrator
{
public:
  static constexpr std::size_t recursion_depth = 10U;

public:
  RecursiveRayTracingIntegrator(World* world)
    : Integrator(world)
  {
  }
  virtual RGBColor getRadiance(const Ray& ray) const;
  RGBColor getRadiance(const Ray& ray, size_t recD) const;
};

}

#endif
