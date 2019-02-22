#include <core/assert.h>
#include <core/scalar.h>
#include <rt/materials/cooktorrance.h>
#include <rt/textures/texture.h>

namespace rt
{

CookTorranceMaterial::CookTorranceMaterial(
  Texture* specular, float eta, float kappa, float m)
  : spec(specular)
  , eta(eta)
  , kappa(kappa)
  , m(m)
{
}
RGBColor CookTorranceMaterial::getReflectance(const Point& texPoint,
  const Vector& normal, const Vector& outDir, const Vector& inDir) const
{
  const Vector H = (outDir - inDir).normalize();

  const float cosF = dot(normal.normalize(), inDir.normalize());
  const float r_par =
    ((sqr(eta) + sqr(kappa)) * sqr(cosF) - 2 * eta * cosF + 1) /
    (((sqr(eta) + sqr(kappa)) * sqr(cosF) + 2 * eta * cosF + 1));

  const float r_ort = ((sqr(eta) + sqr(kappa)) - 2 * eta * cosF + sqr(cosF)) /
                      ((sqr(eta) + sqr(kappa)) + 2 * eta * cosF + sqr(cosF));
  const float fresnelTerm = 0.5f * (r_par + r_ort);

  const float cosNIndir  = dot(normal.normalize(), -inDir.normalize());
  const float cosNOutdir = dot(normal.normalize(), outDir.normalize());
  const float cosNH      = dot(normal.normalize(), H);

  const float denom = pi * cosNIndir * cosNOutdir;
  const float tmp0 = sqr(cosNH) * sqr(cosNH);
  const float tmp1 = std::tan(std::acos(cosNH)) / m;
  const float d     = 1.f / (pi * sqr(m) * tmp0) * std::exp(-sqr(tmp1));
  const float g = std::fmin(
    1.f, std::fmin(2.f * (cosNH * cosNIndir / dot(-inDir.normalize(), H)),
           2.f * (cosNH * cosNOutdir / dot(-inDir.normalize(), H))));

  return spec->getColor(texPoint) * RGBColor::rep(fresnelTerm * d * g / denom);
}
RGBColor CookTorranceMaterial::getEmission(
  const Point& texPoint, const Vector& normal, const Vector& outDir) const
{
  return RGBColor::rep(0.f);
}
Material::SampleReflectance CookTorranceMaterial::getSampleReflectance(
  const Point& texPoint, const Vector& normal, const Vector& outDir) const
{
  return {};
}

Material::Sampling CookTorranceMaterial::useSampling() const
{
  return Material::SAMPLING_NOT_NEEDED;
}
}
