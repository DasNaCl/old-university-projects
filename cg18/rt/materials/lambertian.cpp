//
// Created by Matthias on 13.12.2018.
//

#include <core/assert.h>
#include <rt/materials/lambertian.h>
#include <rt/materials/material.h>
#include <rt/textures/texture.h>
#include <core/scalar.h>

namespace rt
{
LambertianMaterial::LambertianMaterial(Texture* emission, Texture* diffuse)
  : emission(emission)
  , diffuse(diffuse)
{
}
RGBColor LambertianMaterial::getReflectance(const Point& texPoint,
  const Vector& normal, const Vector& outDir, const Vector& inDir) const
{
  float cos = std::abs(dot(outDir.normalize(), normal.normalize()));
  return ((cos * diffuse->getColor(texPoint)) / pi);
}
RGBColor LambertianMaterial::getEmission(
  const Point& texPoint, const Vector& normal, const Vector& outDir) const
{
    return emission->getColor(texPoint);
}
Material::SampleReflectance LambertianMaterial::getSampleReflectance(
  const Point& texPoint, const Vector& normal, const Vector& outDir) const
{
  return {Vector(0,0,0),RGBColor::rep(0.f)};
}
Material::Sampling LambertianMaterial::useSampling() const
{
  return SAMPLING_NOT_NEEDED;
}
}