#include <cmath>
#include <core/random.h>
#include <core/scalar.h>
#include <rt/materials/glass.h>

namespace rt
{

GlassMaterial::GlassMaterial(float eta)
  : eta(eta)
{
}

RGBColor GlassMaterial::getReflectance(const Point& texPoint,
  const Vector& normal, const Vector& outDir, const Vector& inDir) const
{

  // mirror reflects 100%
  return RGBColor::rep(1.f);// *
         //(dot(inDir.normalize(), normal.normalize()));
}

RGBColor GlassMaterial::getEmission(
  const Point& texPoint, const Vector& normal, const Vector& outDir) const
{
  return RGBColor::rep(0.f);
}

Material::SampleReflectance GlassMaterial::getSampleReflectance(
  const Point& texPoint, const Vector& normal, const Vector& outDir) const
{

  const float cosT = dot(outDir.normalize(), normal.normalize());
  const float sinT = std::sqrt(1 - (cosT * cosT));

  const float  ni    = (cosT > 0 + epsilon? 1.f : eta);
  const float  nt    = (cosT > 0 + epsilon? eta : 1.f);
  const float  et    = (cosT > 0 + epsilon? eta : (1.f / eta));
  const float  cosin = std::abs(cosT);
  const Vector relativeNormal =
    (cosT > 0 ? -normal.normalize() : normal.normalize());
  float cosout = sqrt(1 - sqr(et) * sqr(sinT));
  //cosout = ( cosout >0 ? :)
  const float r1     = (nt * cosout - ni * cosin) / (nt * cosout + ni * cosin);
  const float r2     = (ni * cosout - nt * cosin) / (ni * cosout + nt * cosin);

  const float Fr       = 0.5f * (sqr(r1) + sqr(r2));
  const float transmit = (1.f - Fr)/sqr(et);

  Vector inDir = (2 * (dot(normal, -outDir) * normal) + outDir).normalize();

  if(sinT > (1.f / et) )
    return { inDir, { 1, 1, 1 } };
  if(random() < 0.5f)
    return { inDir, RGBColor::rep(2 * Fr) };

  inDir = outDir.normalize() * et + relativeNormal * (et * cosin - cosout);
  return { inDir.normalize(), RGBColor::rep(2 * transmit) };
}

Material::Sampling GlassMaterial::useSampling() const
{
  return Material::SAMPLING_ALL;
}
}
