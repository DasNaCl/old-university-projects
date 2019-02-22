#include <cmath>
#include <core/assert.h>
#include <rt/materials/dummy.h>
#include <rt/materials/material.h>

namespace rt
{

DummyMaterial::DummyMaterial() {}

RGBColor DummyMaterial::getReflectance(const Point& texPoint,
  const Vector& normal, const Vector& outDir, const Vector& inDir) const
{
  // image becomes much, much darker if commented out....
  return RGBColor::rep(std::fabs(dot(inDir, normal)));
}

RGBColor DummyMaterial::getEmission(
  const Point& texPoint, const Vector& normal, const Vector& outDir) const
{
  return RGBColor::rep(0.f);
}

Material::SampleReflectance DummyMaterial::getSampleReflectance(
  const Point& texPoint, const Vector& normal, const Vector& outDir) const
{
  return  {Vector(0,0,0),RGBColor::rep(0.f)};
}
}
