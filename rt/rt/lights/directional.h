#ifndef CG1RAYTRACER_LIGHTS_DIRECTIONAL_HEADER
#define CG1RAYTRACER_LIGHTS_DIRECTIONAL_HEADER

#include <core/color.h>
#include <rt/lights/light.h>

namespace rt
{

class DirectionalLight : public Light
{
public:
  DirectionalLight() {}
  DirectionalLight(const Vector& direction, const RGBColor& color);
  virtual LightHit getLightHit(const Point& p) const;
  virtual RGBColor getIntensity(const LightHit& irr) const;

private:
  Vector   dir;
  RGBColor col;
};
}

#endif
