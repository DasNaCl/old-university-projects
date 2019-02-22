//
// Created by Matthias on 13.12.2018.
//

#include <core/assert.h>
#include <rt/materials/material.h>
#include <rt/materials/mirror.h>
#include <rt/textures/texture.h>
#include <core/scalar.h>

namespace rt
{
MirrorMaterial::MirrorMaterial(float eta, float kappa):
eta(eta),
kappa(kappa)
{
}
RGBColor MirrorMaterial::getReflectance(const Point& texPoint,
  const Vector& normal, const Vector& outDir, const Vector& inDir) const
{
  float cos = dot(inDir,normal);
  float r1 = ((sqr(eta)+sqr(kappa))*sqr(cos)- 2*eta * cos + 1 )/
          ((sqr(eta)+sqr(kappa))*sqr(cos)+ 2*eta * cos + 1);
  float r2 = ((sqr(eta) + sqr(kappa))- 2 * eta * cos + sqr(cos))/
          ((sqr(eta) + sqr(kappa)) + 2 * eta *cos + sqr(cos));
  return RGBColor::rep(0.5f * (r1 + r2));
}
RGBColor MirrorMaterial::getEmission(
  const Point& texPoint, const Vector& normal, const Vector& outDir) const
{
  return RGBColor::rep(0.f);
}
Material::SampleReflectance MirrorMaterial::getSampleReflectance(
  const Point& texPoint, const Vector& normal, const Vector& outDir) const
{
  Vector inDir(outDir+2*(dot(-outDir,normal)*normal));
  return {inDir,getReflectance(texPoint,normal,outDir,inDir)};
}
Material::Sampling MirrorMaterial::useSampling() const
{
  return SAMPLING_ALL;
}
}
