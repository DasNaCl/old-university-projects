//
// Created by Matthias on 30.11.2018.
//

#ifndef CG1_RAYTRACER_PROJECTIVELIGHT_H
#define CG1_RAYTRACER_PROJECTIVELIGHT_H

#include <core/color.h>
#include <core/point.h>
#include <core/scalar.h>
#include <core/vector.h>
#include <rt/lights/light.h>

namespace rt
{
class ProjectiveLight : public Light
{
public:
  ProjectiveLight() {}
  ProjectiveLight(const Point& position, const float width, const float heigth);
  virtual LightHit getLightHit(const Point& p) const;
  virtual RGBColor getIntensity(const LightHit& irr) const;

private:
  float w, h;
  Point pos;
};
}


#endif  //CG1_RAYTRACER_PROJECTIVELIGHT_H
