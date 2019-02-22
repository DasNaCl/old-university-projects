//
// Created by Matthias on 13.12.2018.
//


#include <core/assert.h>
#include <rt/materials/flatmaterial.h>
#include <rt/materials/material.h>
#include <rt/textures/texture.h>

namespace rt
{
FlatMaterial::FlatMaterial(Texture* texture)
  : texture(texture)
{
}
RGBColor FlatMaterial::getReflectance(const Point& texPoint,
  const Vector& normal, const Vector& outDir, const Vector& inDir) const
{
  return { 0, 0, 0 };
}
RGBColor FlatMaterial::getEmission(
  const Point& texPoint, const Vector& normal, const Vector& outDir) const
{
  return texture->getColor(texPoint);
}
Material::SampleReflectance FlatMaterial::getSampleReflectance(
  const Point& texPoint, const Vector& normal, const Vector& outDir) const
{
  return {Vector(0,0,0),{0,0,0}};
}
}