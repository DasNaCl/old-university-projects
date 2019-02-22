#include <rt/lights/projectivecolorlight2.h>
#include <core/scalar.h>
#include <core/interpolate.h>
#include <core/random.h>
#include <core/julia.h>
#include <rt/ray.h>

namespace rt
{

    float a2computeWeight(float fx, float fy, const Point& c, float div) {
        Point v(fx, fy, 0.0f);
        int numIter = julia(v, c);
        return numIter/(numIter+div);
    }

    RGBColor a2computeColor(Ray& r, Ray& r2) {
        float theta = asin(r.d.z)/pi*2;
        float phi = atan2(r.d.y,r.d.x)/pi;

        float theta2 = asin(r2.d.z)/pi*2;
        float phi2 = atan2(r2.d.y,r2.d.x)/pi;

        float ofx = absfractional((r.o.x+1.0f)/2.0f)*2.0f-1.0f;
        float ofy = absfractional((r.o.y+1.0f)/2.0f)*2.0f-1.0f;
        if (absfractional(theta/(2*pi)*90)<0.2) return RGBColor(1,0.5,0);
        if (absfractional(theta2/(2*pi)*90)<0.3) return {0,1,0};
        if (absfractional(phi/(2*pi)*90)<0.09 ) return RGBColor(0.9f,0.9f,0.5f);
        if (absfractional(phi2/(2*pi)*90)<0.09 ) return RGBColor(1,1,1);
        if(absfractional(theta / (2 * pi) * 90) > 0.5) return RGBColor(0.8,1,0);
        return {ofx,ofy,0.5};
    }

    LightHit ProjectiveColorLight2::getLightHit(const Point& p) const
    {
        return { (pos - p).normalize(), (pos - p).length() };
    }

    RGBColor ProjectiveColorLight2::getIntensity(const LightHit& irr) const
    {
        Ray r (pos, -irr.direction - direction1);
        Ray r2(pos, -irr.direction + direction2);

        return 2 * a2computeColor(r,r2);
    }
}