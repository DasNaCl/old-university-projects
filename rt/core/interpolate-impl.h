#include "assert.h"
#include "interpolate.h"

namespace rt
{

template <typename T>
T lerp(const T& px0, const T& px1, float xPoint)
{
  //TODO
  return px0 + xPoint * (px1 - px0);
}

template <typename T>
T lerpbar(const T& a, const T& b, const T& c, float aWeight, float bWeight)
{
  //TODO
  const auto cWeight = 1 - aWeight - bWeight;
  return a * aWeight + b * bWeight + c * cWeight;
}

template <typename T>
T lerp2d(const T& px0y0, const T& px1y0, const T& px0y1, const T& px1y1,
  float xWeight, float yWeight)
{
  //TODO
  const T p1 = lerp(px0y0, px1y0, xWeight);
  const T p2 = lerp(px0y1, px1y1, xWeight);
  return lerp(p1, p2, yWeight);
}

template <typename T>
T lerp3d(const T& px0y0z0, const T& px1y0z0, const T& px0y1z0, const T& px1y1z0,
  const T& px0y0z1, const T& px1y0z1, const T& px0y1z1, const T& px1y1z1,
  float xPoint, float yPoint, float zPoint)
{
  //TODO
  const auto p1 = lerp2d(px0y0z0, px1y0z0, px0y1z0, px1y1z0, xPoint, yPoint);
  const auto p2 = lerp2d(px0y0z1, px1y0z1, px0y1z1, px1y1z1, xPoint, yPoint);
  return lerp(p1, p2, zPoint);
}

}
