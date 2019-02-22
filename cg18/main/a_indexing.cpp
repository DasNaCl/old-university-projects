
#include <core/assert.h>
#include <core/image.h>
#include <core/scalar.h>
#include <rt/cameras/perspective.h>
#include <rt/groups/bvh.h>
#include <rt/integrators/casting.h>
#include <rt/integrators/castingdist.h>
#include <rt/loaders/obj.h>
#include <rt/renderer.h>
#include <rt/solids/sphere.h>
#include <rt/world.h>

#include <rt/solids/triangle.h>

#include <rt/integrators/recraytrace.h>
#include <rt/materials/phong.h>
#include <rt/materials/lambertian.h>
#include <rt/materials/cooktorrance.h>
#include <rt/materials/combine.h>
#include <rt/textures/constant.h>
#include <rt/lights/pointlight.h>

#include <main/scoped_timer.h>

using namespace rt;

void test_obj();

void a_indexing()
{
  Image img(6400, 3600);

  BVH* scene = new BVH();
  scene->add(new Sphere(Point(2.5f, .5f, -1), 0.5, nullptr, nullptr));
  scene->add(new Sphere(Point(2.5f, -1.f, -1), 0.5, nullptr, nullptr));
  scene->add(new Sphere(Point(4.5f, .5f, -1), 0.5, nullptr, nullptr));

  loadOBJ(scene, "models/", "cow.obj");

  scene->rebuildIndex();
  World world;
  world.scene = scene;

  PerspectiveCamera cam1(Point(-8.85f, -7.85f, 7.0f), Vector(1.0f, 1.0f, -0.6f),
    Vector(0, 0, 1), pi / 8, pi / 6);
  PerspectiveCamera cam2(Point(16.065f, -12.506f, 1.771f),
    Point(-0.286f, -0.107f, 1.35f) - Point(16.065f, -12.506f, 1.771f),
    Vector(0, 0, 1), pi / 8, pi / 6);
  RayCastingIntegrator integrator(&world);

#ifdef TIMING
  {
    auto f = [](std::chrono::milliseconds x) {
      std::cout << x.count() << "ms\n";
    };
    ScopedTimer<decltype(f)> s(std::move(f));
#endif
    Renderer engine1(&cam1, &integrator);
    engine1.setImgName("a3-1.png");
    engine1.render(img);
    img.writePNG("a3-1.png");
#ifdef TIMING
  }

  {
    auto f = [](std::chrono::milliseconds x) {
      std::cout << x.count() << "ms\n";
    };
    ScopedTimer<decltype(f)> s(std::move(f));
#endif
    Renderer engine2(&cam2, &integrator);
    engine2.setImgName("a3-2.png");
    engine2.render(img);
    img.writePNG("a3-2.png");
#ifdef TIMING
  }
#endif


#ifdef MODELS
  BVH* scene2 = new BVH();
  loadOBJ(scene2, "models/", "child.obj");

  scene2->rebuildIndex();
  World world2;
  world2.scene = scene2;
  PerspectiveCamera        cam3(130 * Point(-8.85f, -7.85f, 11.0f),
    Vector(2.0f, 2.0f, -2.f), Vector(0, 0, 1), pi / 8, pi / 6);
  RayCastingDistIntegrator integrator2(&world2, RGBColor(1.0f, 0.2f, 0.0f),
    850.0f, RGBColor(0.2f, 1.0f, 0.0f), 4000.0f);


  Renderer engine3(&cam3, &integrator2);
  engine3.setImgName("a3-3.png");
  engine3.render(img);
  img.writePNG("a3-3.png");
#endif

  Texture* blacktex = new ConstantTexture(RGBColor(0.f, 0.f, 0.f));
  Texture* whitetex = new ConstantTexture(RGBColor(1.f, 1.f, 1.f));
  CombineMaterial* material = new CombineMaterial();
  material->add(new LambertianMaterial(blacktex, whitetex), 0.7f);
  material->add(new CookTorranceMaterial(whitetex, 0.4f, 0.3f, 0.02f), 0.3f);

  BVH* scene3 = new BVH();
  {
  BVH* model = new BVH();
  loadOBJ(model, "models/", "lucy.obj");
  std::transform(model->prims.begin(), model->prims.end(), model->prims.begin(),
      [](Primitive* prim)
      {
        auto* tr = static_cast<Triangle*>(prim);
        for(std::size_t i = 0; i < 3; ++i)
        {
          tr->vertices[i].x += 3.f;
          tr->vertices[i].z -= 4.f;
        }
        return tr;
      });
  model->rebuildIndex();
  scene3->add(model);
  model->setMaterial(material);
  }
  {
  BVH* model = new BVH();
  loadOBJ(model, "models/", "lucy.obj");
  model->rebuildIndex();
  scene3->add(model);
  model->setMaterial(material);
  }


  World world3;
  world3.light.push_back(new PointLight(Point(4.07625f, 5.90386f, 2.81311f),
                                        RGBColor::rep(50.f)));
  world3.scene = scene3;
  PerspectiveCamera camm(Point(6.93856f, 4.91192f, 4.98563f),
      Vector(-0.6095f, -0.3029f, -0.7327f), Vector(-0.3031f, 0.9430f, -0.1377f),
      3*pi / 16.f, 3*pi / 9.f);// 0.85695f, pi + 0.85695f * (9.f / 16.f));
  RecursiveRayTracingIntegrator integratorr(&world3);
  Renderer enginee(&camm, &integratorr);
  enginee.setImgName("a3-4.png");
  {
    auto f = [](std::chrono::milliseconds x) {
      std::cout << "Lucy took " << x.count() << "ms\n";
    };
    ScopedTimer<decltype(f)> s(std::move(f));
  scene3->rebuildIndex();
  enginee.render(img);
  img.writePNG("a3-4.png");
  }

  test_obj();
}

void test_obj()
{
  Image img(1600, 900);
  BVH* model = new BVH();
  loadOBJ(model, "models/", "test_obj.obj");
  model->rebuildIndex();

  Texture* blacktex = new ConstantTexture(RGBColor(0.f, 0.f, 0.f));
  Texture* whitetex = new ConstantTexture(RGBColor(1.f, 1.f, 1.f));
  Material* material = new LambertianMaterial(blacktex, whitetex);
  model->setMaterial(material);

  BVH* scene3 = new BVH();
  scene3->add(model);
  scene3->rebuildIndex();

  World world3;
  world3.light.push_back(new PointLight(Point(-3.97780f, 4.07625f, 5.90386f),
                                        RGBColor::rep(100.f)));
  world3.scene = scene3;
  PerspectiveCamera camm(Point(7.48113f, 4.15808f, 4.37833f),
      Vector(-0.6516f, 0.6142f, -0.4453f), Vector(0.3240f, -0.3054f, 0.4954f),
      0.87266f, 0.87266f * (16.f / 9.f));
  RecursiveRayTracingIntegrator integratorr(&world3);
  Renderer enginee(&camm, &integratorr);
  enginee.render(img);
  img.writePNG("a3-5.png");
}
