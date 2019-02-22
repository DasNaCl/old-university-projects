#include <core/assert.h>
#include <numeric>
#include <rt/bbox.h>
#include <rt/groups/simplegroup.h>
#include <rt/intersection.h>

namespace rt
{

BBox SimpleGroup::getBounds() const
{
  return std::accumulate(objects.begin(), objects.end(), BBox::empty(),
    [](BBox currentBox, Primitive* obj) {
      currentBox.extend(obj->getBounds());
      return currentBox;
    });
}

Intersection SimpleGroup::intersect(
  const Ray& ray, float previousBestDistance) const
{
  return std::accumulate(objects.begin(), objects.end(),
    Intersection::failure(),
    [&ray, &previousBestDistance](
      const Intersection& currentInter, Primitive* obj) {
      Intersection intermediateInter =
        obj->intersect(ray, previousBestDistance);
      if(0.f < intermediateInter.distance &&
         intermediateInter.distance < previousBestDistance)
      {
        previousBestDistance = intermediateInter.distance;
        return intermediateInter;
      }
      else if(intermediateInter.distance ==
                std::numeric_limits<float>::infinity() &&
              previousBestDistance == FLT_MAX)
      {
        return intermediateInter;
      }
      return currentInter;
    });
}

void SimpleGroup::rebuildIndex()
{
  // no indexing, so this is a noop
}

void SimpleGroup::add(Primitive* p)
{
  objects.push_back(p);
}

void SimpleGroup::setMaterial(Material* m)
{
  for(auto& o : objects)
    o->setMaterial(m);
}

void SimpleGroup::setCoordMapper(CoordMapper* cm)
{
  for(auto& o : objects)
    o->setCoordMapper(cm);
}
void SimpleGroup::setLastMaterial(Material* m){
   for(auto i = lastNum; i < objects.size(); ++i){
         objects[i]->setMaterial(m);
     }
        lastNum = objects.size();
   }
}
