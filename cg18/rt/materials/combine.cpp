//
// Created by Matthias on 13.12.2018.
//

#include <core/assert.h>
#include <rt/materials/combine.h>
#include <rt/materials/material.h>
#include <rt/textures/texture.h>

namespace rt
{
CombineMaterial::CombineMaterial()
{
  sumWeight = 0.f;
}
void CombineMaterial::add(Material* material, float weight)
{
  sumWeight += weight;
  materials.push_back(std::pair<float, Material*>(weight, material));
}
RGBColor CombineMaterial::getReflectance(const Point& texPoint,
  const Vector& normal, const Vector& outDir, const Vector& inDir) const
{
  RGBColor col(0, 0, 0);
  for(auto& mat : materials)
  {
    Material* m = mat.second;
    col         = col + (mat.first / sumWeight) *
                  (m->getReflectance(texPoint, normal, outDir, inDir));
  }
  return col;
}
RGBColor CombineMaterial::getEmission(
  const Point& texPoint, const Vector& normal, const Vector& outDir) const
{
  RGBColor col(0, 0, 0);
  for(auto& mat : materials)
  {
    Material* m = mat.second;
    col         = col +
          (mat.first / sumWeight) * (m->getEmission(texPoint, normal, outDir));
  }
  return col;
}
Material::SampleReflectance CombineMaterial::getSampleReflectance(
  const Point& texPoint, const Vector& normal, const Vector& outDir) const
{
  return Material::SampleReflectance(Vector(0,0,0),RGBColor::rep(0.f));;
}
Material::Sampling CombineMaterial::useSampling() const
{
  bool has_sec = false;
  bool has_mir = false;
  bool has_lamb = false;
  for(auto& mat : materials)
  {
    switch(mat.second->useSampling())
    {
    case SAMPLING_NOT_NEEDED: has_lamb = true; break;
    case SAMPLING_ALL: has_mir = true; break;
    case SAMPLING_SECONDARY: has_sec = true; break;
    default: break;
    }
  }
  if(!has_lamb && !has_sec && has_mir)
    return SAMPLING_ALL;
  if((has_lamb && has_mir) || has_sec)
    return  SAMPLING_SECONDARY;
  return SAMPLING_NOT_NEEDED;
}
}
