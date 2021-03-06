#ifndef CG1RAYTRACER_CAMERAS_ORTHOGRAPHIC_HEADER
#define CG1RAYTRACER_CAMERAS_ORTHOGRAPHIC_HEADER

#include <rt/cameras/camera.h>
#include <core/vector.h>
#include <core/point.h>

namespace rt {

class OrthographicCamera : public Camera {
public:
    OrthographicCamera(
        const Point& center,
        const Vector& forward,
        const Vector& up,
        float scaleX,
        float scaleY
        );

    Ray getPrimaryRay(float x, float y) const override;
private:
    Point center;
    Vector forward;
    Vector right;
    Vector sup;
    float scaleX;
    float scaleY;
};

}


#endif
