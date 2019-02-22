#include <core/interpolate.h>
#include <core/point.h>
#include <core/scalar.h>
#include <rt/textures/perlin.h>
#include <math.h>
namespace rt
{

namespace
{

/* returns a value in range -1 to 1 */
float noise(int x, int y, int z)
{
  int n = x + y * 57 + z * 997;
  n     = (n << 13) ^ n;
  return (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) /
                   1073741824.0f);
}

}
PerlinTexture::PerlinTexture(const RGBColor& white, const RGBColor& black)
  : white(white)
  , black(black)
{
}
void PerlinTexture::addOctave(float amplitude, float frequency)
{
  octaves.push_back({ amplitude, frequency });
}
RGBColor PerlinTexture::getColor(const Point& coord)
{
  int   floorx, floory, floorz;
  float diffx, diffy, diffz, col;
  col = 0.f;

  for(const auto octave : octaves)
  {
    floorx = static_cast<int>(floorf(coord.x * octave.second));
    diffx  = coord.x * octave.second - floorx;
    floory = static_cast<int>(floorf(coord.y * octave.second));
    diffy  = coord.y * octave.second - floory;
    floorz = static_cast<int>(floorf(coord.z * octave.second));
    diffz  = coord.z * octave.second - floorz;

    float p000 = noise(floorx, floory, floorz);
    float p100 = noise(floorx + 1, floory, floorz);
    float p010 = noise(floorx, floory + 1, floorz);
    float p001 = noise(floorx, floory, floorz + 1);
    float p110 = noise(floorx + 1, floory + 1, floorz);
    float p011 = noise(floorx, floory + 1, floorz + 1);
    float p111 = noise(floorx + 1, floory + 1, floorz + 1);
    float p101 = noise(floorx + 1, floory, floorz + 1);

    col += octave.first * lerp3d(p000, p100, p010, p110, p001, p101, p011, p111,
                            diffx, diffy, diffz);
  }

  return lerp(black, white, (col + 1) / 2);
}
RGBColor PerlinTexture::getColorDX(const Point& coord)
{
  NOT_IMPLEMENTED;
}
RGBColor PerlinTexture::getColorDY(const Point& coord)
{
  NOT_IMPLEMENTED;
}

}
