#ifndef CG1RAYTRACER_GROUPS_BVHBUILDER_HEADER
#define CG1RAYTRACER_GROUPS_BVHBUILDER_HEADER

#include <limits>
#include <rt/bbox.h>
#include <rt/groups/bvh.h>
#include <vector>

namespace rt
{

class Primitive;

enum class PartitionStrategy
{
  SplitInTheMiddle,
  SurfaceAreaHeuristic,
};

struct BuildNode;

class BVHBuilder
{
public:
  struct InfoNode
  {
    InfoNode()
      : primitiveCount(0U)
      , origin(0, 0, 0)
      , bounds(BBox::empty())
    {
    }
    InfoNode(std::size_t primitiveCount, const BBox& bb)
      : primitiveCount(primitiveCount)
      , origin(0.5f * (bb.min.x + bb.max.x), 0.5f * (bb.min.y + bb.max.y),
          0.5f * (bb.min.z + bb.max.z))
      , bounds(bb)
    {
      constexpr static float inf = std::numeric_limits<float>::infinity();
      if(std::isinf(bounds.min.x) || std::isinf(bounds.max.x))
        origin.x = inf;
      if(std::isinf(bounds.min.y) || std::isinf(bounds.max.y))
        origin.y = inf;
      if(std::isinf(bounds.min.z) || std::isinf(bounds.max.z))
        origin.z = inf;
    }

    std::size_t primitiveCount;
    Point       origin;
    BBox        bounds;
  };

public:
  static void run(std::vector<Primitive*>& primitives,
    std::vector<BVH::Node>& treeData, PartitionStrategy part);

private:
  BVHBuilder(std::vector<Primitive*>& primitives,
    std::vector<BVH::Node>& treeData, PartitionStrategy part);
  BuildNode*  recursiveBuild(std::size_t start, std::size_t end);
  std::size_t flatten(BuildNode* root);

private:
  void splitInTheMiddle(std::size_t start, std::size_t& mid, std::size_t end,
    Coordinate dim, BBox origin);
  BuildNode* surfaceAreaHeuristic(std::size_t start, std::size_t& mid,
    std::size_t end, Coordinate dim, BBox origin, std::size_t primitives,
    BBox total);

private:
  std::vector<Primitive*>& prims;
  std::vector<BVH::Node>&  treeData;
  std::vector<InfoNode>    buildData;
  std::vector<Primitive*>  ordered;
  std::size_t              nodeCount;
  std::size_t              offset;

  PartitionStrategy partitionStrategy;
};
}

#endif
