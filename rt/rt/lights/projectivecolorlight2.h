#ifndef CG1_RAYTRACER_PROJECTIVECOLOURLIGHT2_H
#define CG1_RAYTRACER_PROJECTIVECOLOURLIGHT2_H


#include <core/color.h>
#include <core/point.h>
#include "light.h"

namespace rt
{
    class ProjectiveColorLight2 : public Light
    {
    public:
        ProjectiveColorLight2() {}
        ProjectiveColorLight2(const Point& position, const Vector& direction1, const Vector& direction2,const float scale) : pos(position),
        direction1(direction1),
        direction2(direction2),
        scale(scale){};
        virtual LightHit getLightHit(const Point& p) const;
        virtual RGBColor getIntensity(const LightHit& irr) const;

    private:
        Point pos;
        Vector direction1;
        Vector direction2;
        float scale;
    };
}


#endif //CG1_RAYTRACER_PROJECTIVECOLOURLIGHT2_H
