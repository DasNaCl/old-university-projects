#include <algorithm>
#include <cmath>
#include <core/assert.h>
#include <core/scalar.h>
#include <rt/materials/material.h>
#include <rt/materials/phong.h>
#include <rt/textures/texture.h>

namespace rt
{

PhongMaterial::PhongMaterial(Texture* specular, float exponent)
  : spec(specular)
  , exp(exponent)
{
}
RGBColor PhongMaterial::getReflectance(const Point& texPoint,
  const Vector& normal, const Vector& outDir, const Vector& inDir) const
{

  Vector reflectance(outDir + 2 * (dot(-outDir, normal) * normal));
  const float cos =
    std::max(dot(-inDir.normalize(), reflectance.normalize()), 0.f);
  const float middle = (exp + 2) / (2 * pi);
  const float powCos = pow(cos, exp);
  return (spec->getColor(texPoint) * middle * powCos *
          std::abs(dot(normal.normalize(), -outDir.normalize())));
}
RGBColor PhongMaterial::getEmission(
  const Point& texPoint, const Vector& normal, const Vector& outDir) const
{
  return RGBColor::rep(0.f);
}
Material::SampleReflectance PhongMaterial::getSampleReflectance(
  const Point& texPoint, const Vector& normal, const Vector& outDir) const
{
  return Material::SampleReflectance(Vector(0,0,0),RGBColor::rep(0.f));
}
Material::Sampling PhongMaterial::useSampling() const
{
  return SAMPLING_NOT_NEEDED;
}
}
