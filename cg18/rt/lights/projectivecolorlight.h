#ifndef CG1_RAYTRACER_PROJECTIVECOLOURLIGHT_H
#define CG1_RAYTRACER_PROJECTIVECOLOURLIGHT_H


#include <core/color.h>
#include <core/point.h>
#include "light.h"

namespace rt
{
    class ProjectiveColorLight : public Light
    {
    public:
        ProjectiveColorLight() {}
        ProjectiveColorLight(const Point& position, const Vector direction) : pos(position),
        direction(direction){};
        virtual LightHit getLightHit(const Point& p) const;
        virtual RGBColor getIntensity(const LightHit& irr) const;

    private:
        Point pos;
        Vector direction;
    };
}


#endif //CG1_RAYTRACER_PROJECTIVECOLOURLIGHT_H
