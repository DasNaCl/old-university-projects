#include <core/random.h>
#include <core/scalar.h>
#include <math.h>
#include <rt/materials/fuzzymirror.h>

namespace rt
{

FuzzyMirrorMaterial::FuzzyMirrorMaterial(
  float eta, float kappa, float fuzzyangle)
  : eta(eta)
  , kappa(kappa)
  , angle(fuzzyangle)
{
}

RGBColor FuzzyMirrorMaterial::getReflectance(const Point& texPoint,
  const Vector& normal, const Vector& outDir, const Vector& inDir) const
{
  const float cosTheta = dot(inDir.normalize(), normal.normalize());

  const float eta2_kappa2 = sqr(eta) + sqr(kappa);
  const float tmp         = 2.f * eta * cosTheta;

  const float r_par = (eta2_kappa2 * sqr(cosTheta) - tmp + 1.f) /
                      (eta2_kappa2 * sqr(cosTheta) + tmp + 1.f);
  const float r_bot =
    (eta2_kappa2 - tmp + sqr(cosTheta)) / (eta2_kappa2 + tmp + sqr(cosTheta));

  const float fresnel = (r_par + r_bot) / 2.f;

  return RGBColor::rep(fresnel) *
         std::fabs(dot(-outDir.normalize(), normal.normalize()));
}

RGBColor FuzzyMirrorMaterial::getEmission(
  const Point& texPoint, const Vector& normal, const Vector& outDir) const
{
  return RGBColor::rep(0.f);
}

Material::SampleReflectance FuzzyMirrorMaterial::getSampleReflectance(
  const Point& texPoint, const Vector& normal, const Vector& outDir) const
{
  const auto vz = 2 * (dot(normal, -outDir) * normal) + outDir;
  const auto vy = cross(vz, { 0, 1, 0 }).normalize();
  const auto vx = cross({ 1, 0, 0 }, vz).normalize();

  const float r0 = rt::random(0.f, 1.f);
  const float r1 = rt::random(0.f, 1.f);

  const float radius = std::tan(angle) * sqrt(r0);

  const float phi    = 2.f * pi * r1;

  const float x1 = radius * std::cos(phi);
  const float x2 = radius * std::sin(phi);

  const Vector nd = vz + x1 * vx + x2 * vy;

  return SampleReflectance{ nd, getReflectance(texPoint, normal, nd, outDir) };
}

Material::Sampling FuzzyMirrorMaterial::useSampling() const
{
  return Material::SAMPLING_ALL;
}
}
