#ifndef CG1RAYTRACER_LIGHTS_SPOTLIGHT_HEADER
#define CG1RAYTRACER_LIGHTS_SPOTLIGHT_HEADER

#include <core/color.h>
#include <core/point.h>
#include <core/scalar.h>
#include <core/vector.h>
#include <rt/lights/light.h>

namespace rt
{

class SpotLight : public Light
{
public:
  SpotLight() {}
  SpotLight(const Point& position, const Vector& direction, float angle,
    float exp, const RGBColor& intensity);
  virtual LightHit getLightHit(const Point& p) const;
  virtual RGBColor getIntensity(const LightHit& irr) const;

private:
  Point    pos;
  Vector   dir;
  float    angle;
  float    cosphi;
  float    exp;
  RGBColor intensity;
};
}

#endif
