#include <rt/solids/solid.h>

namespace rt
{

Solid::Solid(CoordMapper* texMapper, Material* material)
  : material(material)
  , texMapper(texMapper)
{
}
}
