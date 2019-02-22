#include <cmath>
#include <rt/bbox.h>
#include <core/vector.h>
#include <rt/intersection.h>
#include <rt/primmod/instance.h>

namespace rt
{

static auto rotationX = [](float alpha) {
  return Matrix(Float4(1, 0, 0, 0),
    Float4(0, std::cos(alpha), -std::sin(alpha), 0),
    Float4(0, std::sin(alpha), std::cos(alpha), 0), Float4(0, 0, 0, 1));
};
static auto rotationY = [](float beta) {
  return Matrix(Float4(std::cos(beta), 0, std::sin(beta), 0),
    Float4(0, 1, 0, 0), Float4(-std::sin(beta), 0, std::cos(beta), 0),
    Float4(0, 0, 0, 1));
};
static auto rotationZ = [](float gamma) {
  return Matrix(Float4(std::cos(gamma), -std::sin(gamma), 0, 0),
    Float4(std::sin(gamma), std::cos(gamma), 0, 0), Float4(0, 0, 1, 0),
    Float4(0, 0, 0, 1));
};

Instance::Instance(Primitive* content)
  : inverse(Matrix::identity())
  , transformation(Matrix::identity())
  , prim(content)
{
}

Primitive* Instance::content()
{
  return prim;
}

void Instance::reset()
{
  transformation = Matrix::identity();
  inverse        = transformation.invert();
}

void Instance::translate(const Vector& t)
{
  transformation [0][3] += t.x;
  transformation [1][3] += t.y;
  transformation [2][3] += t.z;
  inverse = transformation.invert();
}

void Instance::rotate(const Vector& axis, float angle)
{

  const Vector ax     = axis.normalize();
  const float  cos    = std::cos(angle);
  const float  cosInv = 1.f - cos;
  const float  sin    = std::sin(angle);
  Matrix       m =
    Matrix(Float4(ax.x * ax.x * cosInv + cos, ax.x * ax.y * cosInv - ax.z * sin,
             ax.z * ax.x * cosInv + ax.y * sin, 0),
      Float4(ax.y * ax.x * cosInv + ax.z * sin, ax.y * ax.y * cosInv + cos,
        ax.z * ax.y * cosInv - ax.x * sin, 0),
      Float4(ax.z * ax.x * cosInv - ax.y * sin,
        ax.z * ax.y * cosInv + ax.x * sin, ax.z * ax.z * cosInv + cos, 0),
      Float4::rep(0));
  m[3][3]        = 1;
  transformation = product(m, transformation);
  inverse        = transformation.invert();
}

void Instance::scale(float scale)
{
  for(std::size_t i = 0U; i < 3U; ++i)
    transformation[i][i] *= scale;
  inverse = transformation.invert();
}

void Instance::scale(const Vector& vec)
{
  Matrix id  = Matrix::identity();
  id [0][0] = vec.x;
  id [1][1] = vec.y;
  id [2][2] = vec.z;
transformation = product(id,transformation);
  inverse = transformation.invert();
}

BBox Instance::getBounds() const
{
  BBox primBox = prim->getBounds();

  primBox.min = transformation * primBox.min;
  primBox.max = transformation * primBox.max;

  return primBox;
}

Intersection Instance::intersect(
  const Ray& ray, float previousBestDistance) const
{
  Vector tr =  inverse * ray.d;

  Ray   transformed = Ray(inverse * ray.o, tr.normalize());

  float newBestDistance =
    (tr.length() / ray.d.length()) * previousBestDistance;

  Intersection inter = prim->intersect(transformed, newBestDistance);

  //transform intersection
  inter.updateNormal((inverse.transpose() * inter.normal()).normalize());
  inter.distance = inter.distance * (ray.d.length() / tr.length());
 inter.ray      = Ray(ray.o,ray.d);
  return inter;
}

void Instance::setMaterial(Material* m)
{
  prim->setMaterial(m);
}

void Instance::setCoordMapper(CoordMapper* cm)
{
  prim->setCoordMapper(cm);
}
}
