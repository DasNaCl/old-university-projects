#include <core/assert.h>
#include <core/color.h>

#include <algorithm>
#include <cmath>

namespace rt
{

RGBColor::RGBColor(const Float4& f4)
  : r()
  , g()
  , b()
{
  NOT_IMPLEMENTED;
}

RGBColor RGBColor::operator+(const RGBColor& c) const
{
  return RGBColor(r + c.r, g + c.g, b + c.b);
}

RGBColor RGBColor::operator-(const RGBColor& c) const
{
  return RGBColor(r - c.r, g - c.g, b - c.b);
}

RGBColor RGBColor::operator*(const RGBColor& c) const
{
  return RGBColor(r * c.r, g * c.g, b * c.b);
}

bool RGBColor::operator==(const RGBColor& b) const
{
  return std::fabs(r - b.r) < 0.000001f && std::fabs(g - b.g) < 0.000001f &&
         std::fabs(this->b - b.b) < 0.000001f;
}

bool RGBColor::operator!=(const RGBColor& b) const
{
  return std::fabs(r - b.r) > 0.000001f || std::fabs(g - b.g) > 0.000001f ||
         std::fabs(this->b - b.b) > 0.000001f;
}

RGBColor RGBColor::clamp() const
{
  return RGBColor(std::max(0.f, std::min(1.f, r)),
    std::max(0.f, std::min(1.f, g)), std::max(0.f, std::min(1.f, b)));
}

RGBColor RGBColor::gamma(float gam) const
{
  NOT_IMPLEMENTED;
}

float RGBColor::luminance() const
{
  NOT_IMPLEMENTED;
}

RGBColor operator*(float scalar, const RGBColor& b)
{
  return RGBColor(scalar * b.r, scalar * b.g, scalar * b.b);
}

RGBColor operator*(const RGBColor& a, float scalar)
{
  return scalar * a;
}

RGBColor operator/(const RGBColor& a, float scalar)
{
  return RGBColor(a.r / scalar, a.g / scalar, a.b / scalar);
}
}
