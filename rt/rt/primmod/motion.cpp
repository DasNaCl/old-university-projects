#include <cmath>
#include <core/vector.h>
#include <rt/bbox.h>
#include <rt/intersection.h>
#include <rt/primmod/motion.h>
#include <rt/renderer.h>

namespace rt
{

Motion::Motion(Primitive* content, Renderer& renderer)
  : acceleration()
  , velocity0()
  , prim(content)
  , renderer(renderer)
{
  renderer.shouldRenderMotion();
}

Primitive* Motion::content()
{
  return prim;
}

void Motion::reset()
{
  acceleration = Vector();
  velocity0    = Vector();
}

void Motion::setMoveRate(const Vector& acceleration, const Vector& velocity0)
{
  this->acceleration = acceleration;
  this->velocity0    = velocity0;
}

BBox Motion::getBounds() const
{
  const float  t      = renderer.getMoveDuration(); // t = MAX
  const Vector newPos = (1.f / 2.f) * acceleration * sqr(t) + velocity0 * t;

  Matrix translated = Matrix::identity();
  translated[0][3] += newPos.x;
  translated[1][3] += newPos.y;
  translated[2][3] += newPos.z;


  BBox box = prim->getBounds(); // t = 0

  box.min = translated * box.min;
  box.max = translated * box.max;

  return box;
}

Intersection Motion::intersect(const Ray& ray, float previousBestDistance) const
{
  const float  t      = renderer.getCurrentTimeStep();
  const Vector newPos = (1.f / 2.f) * acceleration * sqr(t) + velocity0 * t;

  Matrix translated = Matrix::identity();
  translated[0][3] += newPos.x;
  translated[1][3] += newPos.y;
  translated[2][3] += newPos.z;
  Matrix inverse = translated.invert();


  Vector tr = inverse * ray.d;

  Ray transformed = Ray(inverse * ray.o, tr.normalize());

  float newBestDistance = (tr.length() / ray.d.length()) * previousBestDistance;

  Intersection inter = prim->intersect(transformed, newBestDistance);

  //transform intersection
  inter.updateNormal((inverse.transpose() * inter.normal()).normalize());
  inter.distance = inter.distance * (ray.d.length() / tr.length());
  inter.ray      = Ray(ray.o, ray.d);
  return inter;
}

void Motion::setMaterial(Material* m)
{
  prim->setMaterial(m);
}

void Motion::setCoordMapper(CoordMapper* cm)
{
  prim->setCoordMapper(cm);
}
}
