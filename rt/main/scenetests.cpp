#include <core/assert.h>
#include <core/image.h>
#include <core/scalar.h>
#include <rt/renderer.h>
#include <rt/world.h>

#include <cmath>

#include "scoped_timer.h"
#include <chrono>
#include <rt/cameras/dofperspective.h>
#include <rt/cameras/perspective.h>
#include <rt/groups/bvh.h>
#include <rt/groups/simplegroup.h>
#include <rt/integrators/casting.h>
#include <rt/integrators/raytrace.h>
#include <rt/integrators/recraymarching.h>
#include <rt/integrators/recraytrace.h>
#include <rt/integrators/volumetracer.h>
#include <rt/lights/pointlight.h>
#include <rt/lights/projectivecolorlight.h>
#include <rt/loaders/obj.h>
#include <rt/materials/fuzzymirror.h>
#include <rt/materials/cooktorrance.h>
#include <rt/materials/glass.h>
#include <rt/materials/lambertian.h>
#include <rt/primmod/bmap.h>
#include <rt/solids/infiniteplane.h>
#include <rt/solids/sphere.h>
#include <rt/solids/triangle.h>
#include <rt/textures/constant.h>
#include <rt/textures/imagetex.h>
#include <rt/textures/perlin.h>
#include <rt/volumes/heterogenousfog.h>
#include <rt/volumes/uniformfog.h>
#include <rt/volumes/uniformfogambientlight.h>
#include <rt/lights/projectivecolorlight2.h>
#include <core/random.h>
#include <rt/lights/perlinspotlight.h>
#include <rt/lights/spotlight.h>
#include <rt/lights/arealight.h>
#include <rt/materials/combine.h>
#include <rt/coordmappers/plane.h>
#include <rt/lights/directional.h>
#include <rt/materials/mirror.h>

using namespace rt;

void Dinosaurtest2(Volume* fog, bool quick)
{

  Image img(480, 360);
  Image img2(1920, 1440);

  auto Pos = Point(3, 0.3f, 0);

  auto focDist = (5 * Pos - (Point(0, 0, 0))).length() - 1.5;

  PerspectiveCamera cam(
    5 * Pos, { -1, 0.f, -0.f }, { 0, 4, 0 }, pi / 8, pi / 6);
    //{ 6.27474f, 2.93195f, 0.69176f }, { -0.9428f, -0.2849f, -0.1729f }, { -0.2737, 0.9580, -0.0860f },
    //pi / 8, pi / 6);
  DOFPerspectiveCamera dofCam(
    5 * Pos, { -1, 0.f, -0.f }, { 0, 4, 0 }, pi / 8, pi / 6, focDist, 1.f);
    //{ 6.27474f, 2.93195f, 0.69176f }, { -0.9428f, -0.2849f, -0.1729f }, { -0.2737, 0.9580, -0.0860f },
    //pi / 8, pi / 6, focDist, 1.f);
  World world;

  DOFPerspectiveCamera DOFcam2(2.5 * Point(2, 3, -5), (Point(0.5, 0.6, 0) - Point(2, 3, -5)).normalize(), Vector(0, 1, 0), pi / 8, pi / 6,(2.5 * Point(2, 3,
 -5)-Point(0,0,0)).length()+0.5,1.f);
  
  ImageTexture* gras = new ImageTexture("./models/high_grass.png",ImageTexture::MIRROR,ImageTexture::BILINEAR);
  ImageTexture* dirt = new ImageTexture("./models/dirt.png",ImageTexture::MIRROR,ImageTexture::BILINEAR);
  ImageTexture* dino = new ImageTexture("./models/dino_skin_bump.png",ImageTexture::MIRROR,ImageTexture::BILINEAR);
  ImageTexture* dinotex = new ImageTexture("./models/dino_skin.png",ImageTexture::MIRROR,ImageTexture::BILINEAR);

  ImageTexture* water_bump = new ImageTexture("./models/water_bump.png", ImageTexture::REPEAT, ImageTexture::BILINEAR);

  Material* mirror   = new MirrorMaterial(2.485f, 1.433f);
  Texture*  blacktex = new ConstantTexture(RGBColor(-0.2f, -0.2f, -0.2f));
  Texture*  whitetex = new ConstantTexture(RGBColor(.3f, .3f, .3f));
  Texture*  greentex = new ConstantTexture(RGBColor(0.f, 0.4f, 0.f));
  Texture*  swamptex  = new ConstantTexture(RGBColor(0.5647f, 0.6980f, 0.5176f));
  BumpMapper* rest = new BumpMapper(new Triangle({1,0,0},{0,1,0},{0,0,1},nullptr, nullptr),dino,{1,0,0},{0,1,0},{0,0,1},0.5);
  Material* floor1 =  new LambertianMaterial(blacktex, whitetex);
  CombineMaterial* floor2 = new CombineMaterial();
  floor2->add(mirror,0.5);
  floor2->add(floor1,0.5);
  Material* floor3 = new CookTorranceMaterial(whitetex, 0.02f, 0.02f, 0.02f);
  CombineMaterial* floor4 = new CombineMaterial();
  floor4->add(new LambertianMaterial(blacktex, swamptex), 0.6f);
  floor4->add(floor3, 0.4f);
  Material* material1 = new LambertianMaterial(blacktex, whitetex);
  Material* material2 = new LambertianMaterial(blacktex, gras);

  CombineMaterial* dinomat = new CombineMaterial();
  dinomat->add(new CookTorranceMaterial(dinotex, 0.04f, 0.04f, 0.2f), 0.3f);
  dinomat->add(material1, 0.7f);

  PlaneBumpMapper* water = new PlaneBumpMapper(
    new InfinitePlane({ 0, 0.12, 0 }, { 0, 1, 0 }, nullptr, mirror), water_bump, 1000.0f);

  BVH* scene = new BVH();
  //auto* scene = new SimpleGroup();
#define MODELS
#ifdef MODELS

  MatLib* mt = new std::map<std::string, Material*>;
  const auto s ="dino.mtl";
  mt->insert({s,dinomat});
  BVH* dino_mesh = new BVH();
  loadOBJ(dino_mesh, "models/", "dino2.obj");
  std::transform(dino_mesh->prims.begin(), dino_mesh->prims.end(), dino_mesh->prims.begin(),
        [dino](Primitive* prim)
        {
          auto& tr = *((Triangle*) prim);
          const auto bv1 = Point(tr[0].x, tr[0].y, 0.f);
          const auto bv2 = Point(tr[1].x, tr[1].y, 0.f);
          const auto bv3 = Point(tr[2].x, tr[2].y, 0.f);
          return new BumpMapper((Triangle*) prim, dino, bv1, bv2, bv3, 0.08f);
        });

  dino_mesh->rebuildIndex();
  scene->add(dino_mesh);
#endif
  scene->update();
  scene->setMaterial(dinomat);
#ifdef MODELS
  loadOBJ(scene, "models/", "test.obj");
#endif

  scene->setLastMaterial(material2);
  world.scene  = scene;
  world.volume = fog;

  //CoordMapper* mapper = new PlaneCoordMapper(Vector(0, 0, 100), Vector(100, 0, 0));
  scene->add(water);
  /// the eye
  scene->add(
      new Sphere({ 1.56010f, 1.78848f, 1.45616f }, //{ 1.56783f, 1.73136f, 1.70664f },
                        0.08f, nullptr,
                new LambertianMaterial(new ConstantTexture(RGBColor(0.f, 0.f, 0.f)), new ConstantTexture(
                                        RGBColor(0.12f, 0.f, 0.f))))
      );
  scene->rebuildIndex();

  world.light.push_back(
          new PerlinSpotLight({ 4.2, 10, 0.2 },{-0.3f,-1,0.1f},pi / 2, 80.0f,RGBColor(800,600,400) * 0.7f));
          //new PointLight(Point(4.07625, 1.00545, 5.90386), RGBColor(92, 89, 0)));
  world.light.push_back(
          //new DirectionalLight(Vector(-0.2f, -0.5f, -0.5f).normalize(), RGBColor(3,3,3)));
          new SpotLight(5 * Pos + Vector(-5,10,-7),Point(0,0,0)-(5 * Pos + Vector(-5,10,-7)),0.5,pi / 8, RGBColor(50, 50, 50)));
          //new ProjectiveColorLight2(Point(-4, 0.2, 1.f)+Vector(0,0.3,0.3), { 0, 0, 1}, { 0, 1, 0},4.f));
          //new ProjectiveColorLight(Point(0,6,0),{random(),random(),random() }));
          //new PointLight(Point(4.07625,  2.00545, -24.90386), RGBColor(100,100,100)));
  RecursiveRayTracingIntegrator integrator2(&world);
  RecRayMarching                integrator1(&world, true);

  Renderer engine(&cam, &integrator2);
  Renderer engine2(&dofCam, &integrator2);
  Renderer engine3(&cam, &integrator1);
  Renderer engine4(&dofCam, &integrator1);

  engine.setImgName("1.png");
  //engine.render(img2);
  //img2.writePNG("1.png");
  quick ? engine2.setSamples(3) : engine2.setSamples(80);
  quick ? engine4.setSamples(3) : engine4.setSamples(80);
  quick ? engine3.setSamples(3) : engine3.setSamples(80);
/*
  engine2.setImgName("2.png");
  engine2.render(img2);
  img2.writePNG("2.png");

  engine3.setImgName("3.png");
  quick ? engine3.render(img)       :       engine3.render(img2);
  quick ? img.writePNG("3.png")     :       img2.writePNG("3.png");
*/

  quick ? engine4.setImgName("thumbnail.png") : engine4.setImgName("final.png");
  quick ? engine4.render(img)       :       engine4.render(img2);
  quick ? img.writePNG("thumbnail.png")     :       img2.writePNG("final.png");
}

void Dinosaurtest(Volume* fog)
{

  Image             img(400, 300);
  auto              Pos = Point(3, 0.3f, 0);
  PerspectiveCamera cam(
    4 * Pos, { -1, 0.f, -0.f }, { 0, 4, 1 }, pi / 8, pi / 6);
  World world;

  BVH* scene = new BVH();
#ifdef MODELS
  loadOBJ(scene, "models/", "rexobj.obj");
#endif
  Texture*  blacktex = new ConstantTexture(RGBColor(0.f, 0.f, 0.f));
  Texture*  whitetex = new ConstantTexture(RGBColor(1.f, 1.f, 1.f));
  Material* material = new LambertianMaterial(blacktex, whitetex);


  world.scene  = scene;
  world.volume = fog;
  scene->setMaterial(material);
  scene->rebuildIndex();

  world.light.push_back(
    new PointLight(Point(4.07625, 1.00545, 5.90386), RGBColor(100, 50, 50)));
  world.light.push_back(new PointLight(
    100 * Point(4.07625, 1.00545, 5.90386), RGBColor(100000, 5000, 5000)));
  RecursiveRayTracingIntegrator integrator2(&world);
  RecRayMarching                integrator1(&world, true);

  Renderer engine(&cam, &integrator2);
  engine.setSamples(10);
  engine.render(img);
  img.writePNG("rex1.png");
}

void projectiveLighttest()
{
  Image img(4000, 3000);

  PerspectiveCamera cam(
    { 0, 0, 10}, { 0.f, 0.f, -1.f }, { 0, 4, 0 }, pi / 8, pi / 6);
  World world;

  BVH*      scene    = new BVH();
 //auto* scene = new SimpleGroup();
  Texture*  blacktex = new ConstantTexture(RGBColor(0.f, 0.f, 0.f));
  Texture*  whitetex = new ConstantTexture(RGBColor(1.f, 1.f, 1.f));
  Material* material = new LambertianMaterial(blacktex, whitetex);
  world.scene        = scene;
  scene->setMaterial(material);

  scene->add(new InfinitePlane({ 0, 0, 0 }, { 0, 0, 1 }, nullptr, material));
  scene->rebuildIndex();
    //world.light.push_back(new ProjectiveColorLight2({ 0, 0, 10 }, { 0.5f, 1, 1}, { 1, 0.5f, 1},3));
    world.light.push_back(//new ProjectiveColorLight({ 0, 0, 10 }, { random(),random(),random()}));
                new PerlinSpotLight({ 0, 0, 10 },{0,0,-1},pi / 4, 80.0f,{400,200,100}));
  RayTracingIntegrator integrator(&world);
  Renderer             engine(&cam, &integrator);
  engine.render(img);
  img.writePNG("col.png");
}

void iterate(float x, float z, float xx,float zz){
    
  Image img2(1200, 900);
  World world;
  auto Pos = Point(3, 0.3f, 0);

  auto focDist = (5 * Pos - (Point(0, 0, 0))).length();

  PerspectiveCamera cam(
    5 * Pos, { -1, 0.f, -0.f }, { 0, 4, 0 }, pi / 8, pi / 6);
  
  auto* scene = new SimpleGroup();
  #ifdef MODELS
    loadOBJ(scene, "models/", "re.obj");
  #endif

  Texture*  blacktex = new ConstantTexture(RGBColor(0.f, 0.f, 0.f));
  Texture*  whitetex = new ConstantTexture(RGBColor(1.f, 1.f, 1.f));
  Material* material = new LambertianMaterial(blacktex, whitetex);
  Material* mirror   = new FuzzyMirrorMaterial(2.485f, 3.433f, 0.05f);

  world.scene  = scene;
  scene->setMaterial(material);

  scene->add(new InfinitePlane({ 0, 0, 0 }, { 0, 1, 0 }, nullptr, material));
  scene->rebuildIndex();

  world.light.push_back(
          new PerlinSpotLight({ x, 10, z },{xx,-1,zz},pi / 2, 80.0f,{800,600,400}));
  RecursiveRayTracingIntegrator integrator2(&world);
  Renderer engine(&cam, &integrator2);
  engine.render(img2);
  auto title = std::to_string(x) + "_" + std::to_string(z) + "xz_" +std::to_string(xx) + "_" + std::to_string(zz) + "xxzz.png";
  img2.writePNG(title);
  std::cout<< title << std::endl;
    
}

void test()
{
  PerspectiveCamera cam(Point(16.065f, -12.506f, 1.771f),
    Point(-0.286f, -0.107f, 1.35f) - Point(16.065f, -12.506f, 1.771f),
    Vector(0, 0, 1), pi / 8, pi / 6);

  PerlinTexture* perlin =
    new PerlinTexture(RGBColor(0.9f, .89f, 0.2f), RGBColor::rep(0.f));
  perlin->addOctave(0.5f, 5.0f);
  perlin->addOctave(0.25f, 10.0f);
  perlin->addOctave(0.125f, 20.0f);
  perlin->addOctave(0.125f, 40.0f);
  perlin->addOctave(0.125f, 80.0f);
  perlin->addOctave(0.125f, 160.0f);
  //test1(&cam, new HeterogenousFog(perlin, 0.00009f));
  //Dinosaurtest(new HeterogenousFog(perlin, 0.009f));
  //for(float x = -1; x < 1; x = x+0.1f){
    //  for(float z = -1; z< 1; z = z+0.1f ){
      //    iterate(3.4,0.6,x,z);
        //  iterate(4.2,0.2,x,z);
    //}
//}
  //projectiveLighttest();
  Dinosaurtest2(new HeterogenousFog(perlin, 0.09f),false);
}
