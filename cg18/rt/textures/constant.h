#ifndef CG1RAYTRACER_TEXTURES_CONSTANT_HEADER
#define CG1RAYTRACER_TEXTURES_CONSTANT_HEADER

#include <core/vector.h>

#include <core/color.h>
#include <rt/textures/texture.h>

namespace rt
{

class ConstantTexture : public Texture
{
public:
  ConstantTexture();
  ConstantTexture(const RGBColor& color);
  virtual RGBColor getColor(const Point& coord);
  virtual RGBColor getColorDX(const Point& coord);
  virtual RGBColor getColorDY(const Point& coord);

private:
  RGBColor color;
};

}

#endif
