#include <array>
#include <core/assert.h>
#include <rt/groups/bvh.h>
#include <rt/groups/bvhbuilder.h>
#include <rt/intersection.h>

namespace rt
{

BVH::BVH()
  : prims()
  , treeData()
  , bounds(BBox::empty())
{
}

BVH::~BVH()
{

}

BBox BVH::getBounds() const
{
  return treeData.empty() ? BBox::full() : treeData[0].box;
}

Intersection BVH::intersect(const Ray& ray, float previousBestDistance) const
{
  if(treeData.empty())
    return Intersection::failure();
  Intersection hit = Intersection::failure();

  auto dirIsNeg = [ray](Coordinate cord) {
    return (cord == Coordinate::X && ray.d.x < 0) ||
           (cord == Coordinate::Y && ray.d.y < 0) || ray.d.z < 0;
  };
  std::size_t todoOffset  = 0;
  std::size_t currentNode = 0;

  static constexpr std::size_t       todo_size = 1U << 6U;
  std::array<std::size_t, todo_size> todo;
  while(true)
  {
    const Node* node   = &treeData[currentNode];
    const auto  boxcol = node->box.intersect(ray);
    if(boxcol.first <= boxcol.second)
    {
      if(node->primitiveCount > 0)
      {
        for(std::size_t i = 0; i < node->primitiveCount; ++i)
        {
          auto inter =
            prims[node->rOffset + i]->intersect(ray, previousBestDistance);
          if(inter)
          {
            hit                  = inter;
            previousBestDistance = hit.distance;
          }
        }
        if(todoOffset == 0)
          break;
        currentNode = todo[--todoOffset];
        continue;
      }
      if(dirIsNeg(node->axis))
      {
        todo[todoOffset++] = currentNode + 1;
        currentNode        = node->sOffset;
      }
      else
      {
        todo[todoOffset++] = node->sOffset;
        currentNode        = currentNode + 1;
      }
      continue;
    }
    if(todoOffset == 0)
      break;
    currentNode = todo[--todoOffset];
  }
  return hit;
}

void BVH::rebuildIndex()
{
  BVHBuilder::run(prims, treeData, PartitionStrategy::SurfaceAreaHeuristic);
}

void BVH::add(Primitive* p)
{
  prims.push_back(p);
}

void BVH::setMaterial(Material* m)
{
  for(auto* n : prims)
    n->setMaterial(m);
  lastNum = prims.size();
}

void BVH::setLastMaterial(Material* m){
  for(auto i = lastNum; i < prims.size(); ++i){
    prims[i]->setMaterial(m);
  }
  lastNum = prims.size();
}

void BVH::setCoordMapper(CoordMapper* cm)
{
  for(auto* n : prims)
    n->setCoordMapper(cm);
}
void BVH::update() {
  lastNum = prims.size();
}
}
