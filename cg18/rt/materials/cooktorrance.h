#ifndef CG1_RAYTRACER_COOKTORRANCE_H
#define CG1_RAYTRACER_COOKTORRANCE_H

#include <rt/materials/material.h>

namespace rt
{
class Texture;

class CookTorranceMaterial : public Material
{
public:
  CookTorranceMaterial(Texture* specular, float eta, float kappa, float m);

  virtual RGBColor getReflectance(const Point& texPoint, const Vector& normal,
    const Vector& outDir, const Vector& inDir) const;
  virtual RGBColor getEmission(
    const Point& texPoint, const Vector& normal, const Vector& outDir) const;
  virtual SampleReflectance getSampleReflectance(
    const Point& texPoint, const Vector& normal, const Vector& outDir) const;

  virtual Sampling useSampling() const;

private:
  Texture* spec;
  float    eta;
  float    kappa;
  float    m;
};
}


#endif  //CG1_RAYTRACER_COOKTORRANCE_H
