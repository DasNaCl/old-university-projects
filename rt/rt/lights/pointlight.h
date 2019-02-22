#ifndef CG1RAYTRACER_LIGHTS_POINTLIGHT_HEADER
#define CG1RAYTRACER_LIGHTS_POINTLIGHT_HEADER

#include <core/color.h>
#include <core/point.h>
#include <rt/lights/light.h>

namespace rt
{

class PointLight : public Light
{
public:
  PointLight() {}
  PointLight(const Point& position, const RGBColor& intensity);
  virtual LightHit getLightHit(const Point& p) const;
  virtual RGBColor getIntensity(const LightHit& irr) const;

private:
  Point    pos;
  RGBColor intensity;
};
}

#endif
