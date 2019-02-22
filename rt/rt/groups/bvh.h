#ifndef CG1RAYTRACER_GROUPS_BVH_HEADER
#define CG1RAYTRACER_GROUPS_BVH_HEADER

#include <memory>
#include <rt/bbox.h>
#include <rt/groups/group.h>
#include <vector>

namespace rt
{

struct BuildNode;
struct InfoNode;

class BVH : public Group
{
public:
  static constexpr std::size_t leafCountTotal = 4;
  struct Node
  {
    unsigned char primitiveCount;
    Coordinate    axis;
    uint8_t       pad[2];

    union {
      std::uint32_t rOffset;
      std::uint32_t sOffset;
    };
    BBox box;
  };

public:
  BVH();
  virtual BBox         getBounds() const;
  virtual Intersection intersect(
    const Ray& ray, float previousBestDistance = FLT_MAX) const;
  virtual void rebuildIndex();
  virtual ~BVH();
  virtual void add(Primitive* p);
  virtual void setMaterial(Material* m);
  virtual void setLastMaterial(Material* m);
  virtual void setCoordMapper(CoordMapper* cm);
  void update();

  std::vector<Primitive*> prims;
private:
  std::vector<Node>       treeData;
  size_t lastNum = 0U;
  BBox bounds;
};
}

#endif
