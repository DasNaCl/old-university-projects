#include <core/color.h>
#include <core/random.h>
#include <rt/coordmappers/coordmapper.h>
#include <rt/coordmappers/world.h>
#include <rt/integrators/recraymarching.h>
#include <rt/intersection.h>
#include <rt/lights/light.h>
#include <rt/materials/material.h>
#include <rt/solids/solid.h>
#include <rt/volumes/volume.h>
#include <rt/world.h>

namespace rt{

    static thread_local auto defaultMapper = WorldMapper();

    RGBColor RecRayMarching::getRadiance(const Ray& ray) const{
        return getRadiance(ray,0);
    }
    RGBColor RecRayMarching::getRadiance(const Ray& ray, size_t recD) const{

        Intersection intersection = this->world->scene->intersect(ray);

        RGBColor intensity, reflectance;
        RGBColor color = {0,0,0};

        if(!intersection)
            return  getRadiance(ray,maxDist,color);
        auto&      material            = *(intersection.solid->material);
        Material::Sampling sample = (*(intersection.solid->material)).useSampling();
        const auto interPoint = intersection.hitPoint();
        const auto intNorm             = intersection.normal().normalize();
        const auto pointOfIntersection = intersection.hitPoint();

        const auto texPoint =
                intersection.solid->texMapper
                ? intersection.solid->texMapper->getCoords(intersection)
                : defaultMapper.getCoords(intersection);

        color = intersection.solid->material->getEmission(
                texPoint, intersection.normal().normalize(), ray.d.normalize());
        if(sample == Material::SAMPLING_SECONDARY ||
           sample == Material::SAMPLING_NOT_NEEDED)
        {
            for(auto& light : world->light)
            {
                const auto lightHit = light->getLightHit(interPoint);
                const auto lHitNdir = lightHit.direction.normalize();
                const float tmp0 = dot(lHitNdir, intNorm);
                const float tmp1     = dot(-ray.d.normalize(), intNorm);

                if((tmp0 < 0 && tmp1 > 0) || (tmp0 > 0 && tmp1 < 0))
                    continue;

                auto shadowIntersection = world->scene->intersect(
                        //something goes wrong with the upset point
                        { pointOfIntersection - 0.0001 * -lHitNdir, lHitNdir },
                        lightHit.distance - rt::epsilon);
                if(shadowIntersection)
                    continue;

                if(dot(-lightHit.direction, intersection.normal().normalize()) *
                   dot(ray.d.normalize(), intersection.normal().normalize()) <
                   0)
                    continue;

                intensity = light->getIntensity(lightHit);
                reflectance = material.getReflectance(
                        texPoint, intNorm, (lHitNdir.normalize()), ray.d.normalize());

                color = color + (intensity * reflectance);
            }
        }
        if(recD < recDepth && (sample == Material::SAMPLING_SECONDARY ||
                                      sample == Material::SAMPLING_ALL))
        {
            auto r = material.getSampleReflectance(interPoint,intersection.normal(), ray.d);
            auto ref =
                    getRadiance({ interPoint + (0.01 * r.direction), r.direction }, recD + 1);
            color = color + r.reflectance * ref;
        }
        return intersection.distance == std::numeric_limits<float>::infinity() ?
            getRadiance(ray,maxDist,color) :
            getRadiance(ray,intersection.distance,color);

    }
    RGBColor RecRayMarching::getRadiance(const Ray& ray, float distance, RGBColor color) const{

        RGBColor L(0,0,0);
        RGBColor T(1,1,1);
        float    t       = useSampling ? step / random() : 0.f;
        float    newStep = step;
        if(distance)
          while(t < distance)
          {
            RGBColor delta(0,0,0);
            const float dt                 = std::min(newStep, distance - t);
            const Point p                  = ray.o + ((t + dt / 2.f)) * ray.d;
            const float deltaTransmittance = world->volume->getTransmittance(dt);
            for(auto& li : world->light){
                const auto lightHit = li->getLightHit(p);
                const auto lHitNdir = lightHit.direction.normalize();

                if(world->scene->intersect({p,lHitNdir}))continue;

                const float transmittance = world->volume->getTransmittance(lightHit.distance);
                const auto irradiance = li->getIntensity(lightHit) * transmittance;

                delta = delta + irradiance;
            }
            T = deltaTransmittance * T;
            L = L + T * (1 - deltaTransmittance) * delta * world->volume->getColor(p);
            t += (t + 1) * step;
            newStep = (t + 1) * step;
          }
        L = L + T * color;
        return L;
    }


}