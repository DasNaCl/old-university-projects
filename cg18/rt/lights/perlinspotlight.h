#ifndef CG1RAYTRACER_LIGHTS_PERLINSPOTLIGHT_HEADER
#define CG1RAYTRACER_LIGHTS_PERLINSPOTLIGHT_HEADER

#include <core/color.h>
#include <core/point.h>
#include <core/scalar.h>
#include <core/vector.h>
#include <rt/lights/light.h>
#include <rt/textures/perlin.h>

namespace rt
{

class PerlinSpotLight : public Light
{
public:
  PerlinSpotLight() {}
  PerlinSpotLight(const Point& position, const Vector& direction, float angle,
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
    PerlinTexture* perlin;
};
}

#endif
