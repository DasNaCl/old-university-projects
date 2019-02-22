#pragma once

#include <rt/integrators/integrator.h>

namespace rt
{
class World;
class Ray;
class RGBColor;

class VolumeIntegrator : public Integrator
{
public:
  VolumeIntegrator(
    World* world, float maxRayMarchingLen = 5.f, float rayMarchingStep = 0.1f)
    : Integrator(world)
    , maxRayMarchingLen(maxRayMarchingLen)
    , rayMarchingStep(rayMarchingStep)
  {
  }
  virtual RGBColor getRadiance(const Ray& ray) const;

private:
  RGBColor rayMarch(const RGBColor& col, RGBColor T, float distance,
    const Ray& ray, bool light) const;

private:
  float maxRayMarchingLen;
  float rayMarchingStep;
};
}
