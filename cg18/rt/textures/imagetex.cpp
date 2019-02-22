#include <cmath>
#include <core/image.h>
#include <core/interpolate.h>
#include <core/point.h>
#include <core/scalar.h>
#include <rt/textures/imagetex.h>

namespace rt
{
    ImageTexture::ImageTexture() {}
    ImageTexture::ImageTexture(
            const Image& image, BorderHandlingType bh, InterpolationType i)
            : handling(bh)
            , image(image)
            , i(i)
    {
    }
    ImageTexture::ImageTexture(
            const std::string& filename, BorderHandlingType bh, InterpolationType i)
            : handling(bh)
            , image()
            , i(i)
    {
      image.readPNG(filename);
    }
    RGBColor ImageTexture::getColor(const Point& coord)
    {
      int xx = int(floor(coord.x * image.width()));
      int yy = int(floor(coord.y * image.height()));
      switch(i)
      {
        case NEAREST: return nearestInter(xx, yy);
        case BILINEAR:
        {
          const auto p00 = nearestInter(xx, yy);
          const auto p10 = nearestInter(xx + 1, yy);
          const auto p01 = nearestInter(xx, yy + 1);
          const auto p11 = nearestInter(xx + 1, yy + 1);

          return lerp2d(p00, p10, p01, p11, std::abs(coord.x * image.width() - xx),
                        std::abs(coord.y * image.height() - yy));
        }
        default: return RGBColor(248, 24, 148);  // pink
      }
    }

    RGBColor ImageTexture::nearestInter(const int x, const int y) {
      const std::size_t w = image.width();
      const std::size_t h = image.height();

      std::size_t xx = 0U;
      std::size_t yy = 0U;

      switch(handling)
      {
        case CLAMP:
        {
          xx = static_cast<std::size_t>(x) >= w ? w - 1 : x < 0 ? 0 : x;
          yy = static_cast<std::size_t>(y) >= h ? h - 1 : y < 0 ? 0 : y;
          break;
        }
        case MIRROR:
        {
          xx = (x / w) % 2 == 0 ? (x % w) : (w - 1) - (x % w);
          yy = (y / h) % 2 == 0 ? (y % h) : (h - 1) - (y % h);
          break;
        }
        case REPEAT:
        {
          xx = static_cast<std::size_t>(x) % w;
          yy = static_cast<std::size_t>(y) % h;
          break;
        }
        default:break;
      }
      return image(xx, yy);
    }

    RGBColor ImageTexture::getColorDX(const Point& coord)
    {
      const auto upper = getColor({coord.x + .5f/ (image.width()),coord.y ,0});
      const auto lower = getColor({coord.x - .5f/ (image.width()),coord.y ,0});

      return  (upper - lower);
    }
    RGBColor ImageTexture::getColorDY(const Point& coord)
    {
      const auto upper = getColor({coord.x ,coord.y + (.5f /(image.height())),0});
      const auto lower = getColor({coord.x ,coord.y - (.5f /(image.height())),0});

      return  (upper - lower);
    }
}
