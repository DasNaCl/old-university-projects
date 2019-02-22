#include <rt/integrators/integrator.h>
#include <cstddef>

#ifndef CG1_RAYTRACER_RECRAYMARCHING_H
#define CG1_RAYTRACER_RECRAYMARCHING_H

namespace rt {

    class World;
    class Ray;
    class RGBColor;

    class RecRayMarching : public Integrator{
    public:
        static constexpr std::size_t  recDepth = 4U;
        static constexpr float step = 0.1f;
        static constexpr float maxDist = 15.f;
        bool                          useSampling;
        virtual RGBColor getRadiance(const Ray& ray) const;
        RGBColor getRadiance(const Ray& ray, size_t recD) const;
        RGBColor getRadiance(const Ray& ray, float distance, RGBColor color) const;


        RecRayMarching(World* world)
          : Integrator(world)
          , useSampling(false)
        {
                }
                RecRayMarching(World* world, bool sampling)
                  : Integrator(world)
                  , useSampling(sampling)
                {
                }
    };

}


#endif //CG1_RAYTRACER_RECRAYMARCHING_H
