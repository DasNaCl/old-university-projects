#include <array>
#include <core/assert.h>
#include <core/macros.h>
#include <rt/bbox.h>
#include <rt/groups/bvhbuilder.h>

namespace rt
{

struct BuildNode
{
  BuildNode()
    : bounds(BBox::empty())
    , children{ nullptr, nullptr }
    , splitAxis(Coordinate::X)
    , firstOff(0)
    , primitiveCount(0)
  {
  }

  BuildNode(std::size_t first, std::size_t n, const BBox& bb)
    : bounds(bb)
    , children{ nullptr, nullptr }
    , splitAxis(Coordinate::X)
    , firstOff(first)
    , primitiveCount(n)
  {
  }

  BuildNode(Coordinate axis, BuildNode* left, BuildNode* right)
    : bounds(BBox::empty())
    , children{ left, right }
    , splitAxis(axis)
    , firstOff(0)
    , primitiveCount(0)
  {
    bounds.extend(left->bounds);
    bounds.extend(right->bounds);
  }

  BBox        bounds;
  BuildNode*  children[2];
  Coordinate  splitAxis;
  std::size_t firstOff;
  std::size_t primitiveCount;
};

void BVHBuilder::run(std::vector<Primitive*>& primitives,
  std::vector<BVH::Node>& treeData, PartitionStrategy part)
{
  BVHBuilder build(primitives, treeData, part);
}

BVHBuilder::BVHBuilder(std::vector<Primitive*>& primitives,
  std::vector<BVH::Node>& treeData, PartitionStrategy part)
  : prims(primitives)
  , treeData(treeData)
  , buildData()
  , ordered()
  , nodeCount(0U)
  , offset(0U)
  , partitionStrategy(part)
{
  buildData.reserve(primitives.size());
  for(std::size_t i = 0; i < primitives.size(); ++i)
    buildData.push_back(InfoNode(i, primitives[i]->getBounds()));

  auto* root = recursiveBuild(0, primitives.size());
  primitives = ordered;

  treeData.resize(nodeCount);
  flatten(root);

  assert(offset == nodeCount);
}

BuildNode* BVHBuilder::recursiveBuild(std::size_t start, std::size_t end)
{
  // build tree in 'recursive manner'
  ++nodeCount;

  BBox bbox = BBox::empty();
  for(std::size_t i = start; i < end; ++i)
    bbox.extend(buildData[i].bounds);

  const std::size_t primitives = end - start;
  if(primitives <= BVH::leafCountTotal)
  {
    const std::size_t firstPrimOffset = ordered.size();
    std::transform(buildData.begin() + start, buildData.begin() + end,
      std::back_inserter(ordered),
      [this](const InfoNode& info) { return prims[info.primitiveCount]; });
    return new BuildNode(firstPrimOffset, primitives, bbox);
  }
  BBox origin = BBox::empty();
  for(std::size_t i = start; i < end; ++i)
    origin.extend(buildData[i].origin);

  const auto       diag = origin.diagonal();
  const Coordinate dim  = (diag.x > diag.y && diag.x > diag.z
                            ? Coordinate::X
                            : diag.y > diag.z ? Coordinate::Y : Coordinate::Z);
  std::size_t      mid  = (start + end) / 2;
  if(std::fabs(origin.max[dim] - origin.min[dim]) <= rt::epsilon)
  {
    const std::size_t firstPrimOffset = ordered.size();
    std::transform(buildData.begin() + start, buildData.begin() + end,
      std::back_inserter(ordered),
      [this](const InfoNode& info) { return prims[info.primitiveCount]; });
    return new BuildNode(firstPrimOffset, primitives, bbox);
  }

  switch(partitionStrategy)
  {
  default:
  case PartitionStrategy::SplitInTheMiddle:
    splitInTheMiddle(start, mid, end, dim, origin);
    break;
  case PartitionStrategy::SurfaceAreaHeuristic:
    auto* n =
      surfaceAreaHeuristic(start, mid, end, dim, origin, primitives, bbox);
    if(n)
    {
      n->splitAxis   = dim;
      n->children[0] = recursiveBuild(start, mid);
      n->children[1] = recursiveBuild(mid, end);
      n->bounds.extend(n->children[0]->bounds);
      n->bounds.extend(n->children[1]->bounds);
      n->primitiveCount = 0U;

      return n;
    }
  }
  return new BuildNode(
    dim, recursiveBuild(start, mid), recursiveBuild(mid, end));
}

std::size_t BVHBuilder::flatten(BuildNode* root)
{
  // now flatten it for efficiency into a single, contigious array
  auto* linearNode = &treeData[offset];
  linearNode->box  = root->bounds;

  const std::size_t myOffset = offset++;
  if(root->primitiveCount > 0)
  {
    linearNode->rOffset        = root->firstOff;
    linearNode->primitiveCount = root->primitiveCount;
  }
  else
  {
    linearNode->axis           = root->splitAxis;
    linearNode->primitiveCount = 0;

    flatten(root->children[0]);

    linearNode->sOffset = flatten(root->children[1]);
  }
  delete root;
  return myOffset;
}
void BVHBuilder::splitInTheMiddle(std::size_t start, std::size_t& mid,
  std::size_t end, Coordinate dim, BBox origin)
{
  const float     fmid = .5f * (origin.min[dim] + origin.max[dim]);
  const InfoNode* midPtr =
    std::partition(&buildData[start], &buildData[end - 1] + 1,
      [dim, fmid](const InfoNode& r) { return r.origin[dim] < fmid; });
  mid = midPtr - &buildData[0];
}
struct Bucket
{
  Bucket()
    : c(0)
    , box(BBox::empty())
  {
  }
  std::size_t c;
  BBox        box;
};
BuildNode* BVHBuilder::surfaceAreaHeuristic(std::size_t start, std::size_t& mid,
  std::size_t end, Coordinate dim, BBox origin, std::size_t primitives,
  BBox total)
{
  if(primitives <= BVH::leafCountTotal)
  {
    mid = (start + end) / 2;
    std::nth_element(&buildData[start], &buildData[mid],
      &buildData[end - 1] + 1, [dim](const InfoNode& a, const InfoNode& b) {
        return a.origin[dim] < b.origin[dim];
      });
    return nullptr;
  }
  constexpr static std::size_t    bucketCount = 12;
  std::array<Bucket, bucketCount> buckets;
  for(std::size_t i = start; i < end; ++i)
  {
    const float perc = (buildData[i].origin[dim] - origin.min[dim]) /
                       (origin.max[dim] - origin.min[dim]);
    std::size_t index = perc * bucketCount;
    if(index >= bucketCount)
      index = bucketCount - 1;
    assert(0 <= index && index < bucketCount);

    buckets[index].c++;
    buckets[index].box.extend(buildData[i].bounds);
  }
  std::array<float, bucketCount - 1> costs;
  for(std::size_t b = 0; b < bucketCount - 1; ++b)
  {
    Bucket b0;
    Bucket b1;
    for(std::size_t j = 0; j <= b; ++j)
    {
      b0.box.extend(buckets[j].box);
      b0.c += buckets[j].c;
    }
    for(std::size_t j = b + 1; j < bucketCount; ++j)
    {
      b1.box.extend(buckets[j].box);
      b1.c += buckets[j].c;
    }
    const auto diag0 = b0.box.diagonal();
    const auto diag1 = b1.box.diagonal();
    const auto tot   = total.diagonal();
    costs[b] =
      0.125f +
      (b0.c * 2 * (diag0.x * diag0.y + diag0.x * diag0.z + diag0.y * diag0.z) +
        b1.c * 2 *
          (diag1.x * diag1.y + diag1.x * diag1.z + diag1.y * diag1.z)) /
        (2 * (tot.x * tot.y + tot.x * tot.z + tot.y * tot.z));
  }
  float       min    = costs[0];
  std::size_t minInd = 0U;
  for(std::size_t i = 1U; i < bucketCount - 1; ++i)
  {
    if(costs[i] < min)
    {
      min    = costs[i];
      minInd = i;
    }
  }

  if(primitives > BVH::leafCountTotal || min < primitives)
  {
    InfoNode* p = std::partition(&buildData[start], &buildData[end - 1] + 1,
      [minInd, dim, origin](const InfoNode& node) {
        const float perc = (node.origin[dim] - origin.min[dim]) /
                           (origin.max[dim] - origin.min[dim]);
        std::size_t b = bucketCount * perc;
        if(b >= bucketCount)
          b = bucketCount - 1;
        assert(0 <= b && b < bucketCount);
        return b < minInd;
      });
    mid         = p - &buildData[0];
    // check for bad split and just choose the center then
    if(mid == start || mid == end)
      mid = start + (end - start) / 2;
  }
  else
  {
    const std::size_t firstPrimOffset = ordered.size();
    std::transform(buildData.begin() + start, buildData.begin() + end,
      std::back_inserter(ordered),
      [this](const InfoNode& info) { return prims[info.primitiveCount]; });
    return new BuildNode(firstPrimOffset, primitives, total);
  }
  return nullptr;
}
}
