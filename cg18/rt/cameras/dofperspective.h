#ifndef CG1RAYTRACER_CAMERAS_DOFPERSPECTIVE_HEADER
#define CG1RAYTRACER_CAMERAS_DOFPERSPECTIVE_HEADER

#include <core/point.h>
#include <core/vector.h>
#include <rt/cameras/camera.h>

namespace rt
{

class Point;
class Vector;

class DOFPerspectiveCamera : public Camera
{
public:
  DOFPerspectiveCamera(const Point& center, const Vector& forward,
    const Vector& up, float verticalOpeningAngle, float horizonalOpeningAngle,
    float focalDistance, float apertureRadius);

  virtual Ray getPrimaryRay(float x, float y) const;

private:
  const Point  center;
  const Vector focal;
  const Vector right;
  const Vector sup;
  const float  ratio;
  const float  angle;
  const float  focus;
  const float  focalDist;
  const float  apertureRadius;
};
}


#endif
