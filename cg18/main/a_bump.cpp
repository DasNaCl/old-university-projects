
#include <core/assert.h>
#include <core/scalar.h>
#include <core/image.h>
#include <rt/world.h>
#include <rt/renderer.h>

#include <cmath>

#include <rt/groups/simplegroup.h>
#include <rt/cameras/perspective.h>
#include <rt/primmod/bmap.h>
#include <rt/textures/imagetex.h>
#include <rt/solids/triangle.h>
#include <rt/solids/sphere.h>
#include <rt/integrators/casting.h>

#include <rt/groups/bvh.h>
#include <rt/loaders/obj.h>
#include <rt/materials/lambertian.h>
#include <rt/textures/constant.h>
#include <rt/textures/perlin.h>
#include <rt/lights/pointlight.h>
#include <rt/integrators/volumetracer.h>
#include <rt/volumes/uniformfogambientlight.h>
#include <rt/volumes/heterogenousfog.h>
#include <rt/volumes/uniformfog.h>
#include <rt/materials/glass.h>
#include <rt/integrators/recraymarching.h>
#include <rt/solids/infiniteplane.h>

using namespace rt;

void bump(Camera* cam, Texture* bumptex, const char* filename) {
    static const float scale = 0.001f;
    static const float bumbscale = 0.008f;
    Image img(800, 800);
    World world;
    SimpleGroup* scene = new SimpleGroup();
    world.scene = scene;

    

    //floor
    scene->add(
        new BumpMapper(
            new Triangle(Point(000.f,000.f,000.f)*scale, Point(000.f,000.f,560.f)*scale, Point(550.f,000.f,000.f)*scale, nullptr, nullptr),
            bumptex, Point(0.0f,0.0f,0.0f), Point(0.0f, 1.0f, 0.0f), Point(1.0f, 0.0f, 0.0f), bumbscale)
            );
    scene->add(
        new BumpMapper(
            new Triangle(Point(550.f,000.f,560.f)*scale, Point(550.f,000.f,000.f)*scale, Point(000.f,000.f,560.f)*scale, nullptr, nullptr),
            bumptex, Point(1.0f,1.0f,0.0f), Point(1.0f, 0.0f, 0.0f), Point(0.0f, 1.0f, 0.0f), bumbscale)
            ); 

    //ceiling
    scene->add(
        new BumpMapper(
            new Triangle(Point(000.f,550.f,000.f)*scale, Point(550.f,550.f,000.f)*scale, Point(000.f,550.f,560.f)*scale, nullptr, nullptr),
            bumptex, Point(0.0f,0.0f,0.0f), Point(1.0f, 0.0f, 0.0f), Point(0.0f, 1.0f, 0.0f), bumbscale)
        ); 
    scene->add(
        new BumpMapper(
            new Triangle(Point(550.f,550.f,560.f)*scale, Point(000.f,550.f,560.f)*scale, Point(550.f,550.f,000.f)*scale, nullptr, nullptr),
            bumptex, Point(1.0f,1.0f,0.0f), Point(0.0f, 1.0f, 0.0f), Point(1.0f, 0.0f, 0.0f), bumbscale)
        ); 

    //back wall
    scene->add(
        new BumpMapper(
            new Triangle(Point(000.f,000.f,560.f)*scale, Point(000.f,550.f,560.f)*scale, Point(550.f,000.f,560.f)*scale, nullptr, nullptr),
            bumptex, Point(0.0f,0.0f,0.0f), Point(0.0f, 1.0f, 0.0f), Point(1.0f, 0.0f, 0.0f), bumbscale)
        ); 
    scene->add(
        new BumpMapper(
            new Triangle(Point(550.f,550.f,560.f)*scale, Point(550.f,000.f,560.f)*scale, Point(000.f,550.f,560.f)*scale, nullptr, nullptr),
            bumptex, Point(1.0f,1.0f,0.0f), Point(1.0f, 0.0f, 0.0f), Point(0.0f, 1.0f, 0.0f), bumbscale)
            ); 

    //right wall
    scene->add(
        new BumpMapper(
            new Triangle(Point(000.f,000.f,000.f)*scale, Point(000.f,550.f,000.f)*scale, Point(000.f,000.f,560.f)*scale, nullptr, nullptr),
            bumptex, Point(0.0f,0.0f,0.0f), Point(0.0f, 1.0f, 0.0f), Point(1.0f, 0.0f, 0.0f), bumbscale)
        );
    scene->add(
        new BumpMapper(
            new Triangle(Point(000.f,550.f,560.f)*scale, Point(000.f,000.f,560.f)*scale, Point(000.f,550.f,000.f)*scale, nullptr, nullptr),
            bumptex, Point(1.0f,1.0f,0.0f), Point(1.0f, 0.0f, 0.0f), Point(0.0f, 1.0f, 0.0f), bumbscale)
            );

    //left wall
    scene->add(
        new BumpMapper(
            new Triangle(Point(550.f,000.f,000.f)*scale, Point(550.f,000.f,560.f)*scale, Point(550.f,550.f,000.f)*scale, nullptr, nullptr),
            bumptex, Point(0.0f,0.0f,0.0f), Point(1.0f, 0.0f, 0.0f), Point(0.0f, 1.0f, 0.0f), bumbscale)
            ); 
    scene->add(
        new BumpMapper(
            new Triangle(Point(550.f,550.f,560.f)*scale, Point(550.f,550.f,000.f)*scale, Point(550.f,000.f,560.f)*scale, nullptr, nullptr),
            bumptex, Point(1.0f,1.0f,0.0f), Point(0.0f, 1.0f, 0.0f), Point(1.0f, 0.0f, 0.0f), bumbscale)
        ); 

    RayCastingIntegrator integrator(&world);

    Renderer engine(cam, &integrator);
    engine.render(img);
    engine.setSamples(9);
    img.writePNG(filename);
}

void fog(Camera* cam, Volume* fog, const std::string& filename)
{
  Image img(800, 600);
  BVH* scene = new BVH();
  scene->add(new Sphere(Point(2.5f, .5f, -1), 0.5, nullptr, nullptr));
  scene->add(new Sphere(Point(2.5f, -1.f, -1), 0.5, nullptr, nullptr));
  scene->add(new Sphere(Point(4.5f, .5f, -1), 0.5, nullptr, nullptr));

  loadOBJ(scene, "models/", "cow.obj");

  scene->rebuildIndex();
  World world;
  world.scene = scene;

  Texture* blacktex = new ConstantTexture(RGBColor(0.f, 0.f, 0.f));
  Texture* whitetex = new ConstantTexture(RGBColor(1.f, 1.f, 1.f));
  Material* material = new LambertianMaterial(blacktex, whitetex);
  scene->setMaterial(material);
  world.light.push_back(new PointLight(Point(15.065f, -11.506f, 1.371f),
                                        RGBColor::rep(1000.f)));

  world.volume = fog;
  VolumeIntegrator fogint(&world);
  Renderer engine(cam, &fogint);
  engine.setSamples(1);
  engine.render(img);
  img.writePNG(filename);
}

void fog2(Camera* cam, Volume* fog, const std::string& filename)
{
    Image img(400, 300);

    BVH* scene = new BVH();

    loadOBJ(scene, "models/", "cow.obj");

    scene->rebuildIndex();
    World world;
    world.scene = scene;

    Texture* blacktex = new ConstantTexture(RGBColor(0.f, 0.f, 0.f));
    Texture* whitetex = new ConstantTexture(RGBColor(1.f, 1.f, 1.f));
    Material* material = new LambertianMaterial(blacktex, whitetex);

    scene->setMaterial(material);
    Material* glass = new GlassMaterial(2.0f);
    scene->add(new Sphere(Point(2.5f, .5f, 1), 1.f, nullptr, glass));

    //scene->add(new Sphere(Point(2.5f, -1.f, 1), 0.5, nullptr, glass));
    //scene->add(new Sphere(Point(4.5f, .5f, 1), 0.5, nullptr, material));
    world.light.push_back(new PointLight(Point(.065f, .506f, 1.371f),
                                         RGBColor::rep(1000.f)));
    scene->rebuildIndex();
    world.volume = fog;
    RecRayMarching fogint(&world);
    Renderer engine(cam, &fogint);
    engine.setSamples(1);
    engine.render(img);
    img.writePNG(filename);
}


void a_bumpmappers() {
  ImageTexture* bumptex = new ImageTexture("models/stones_bump.png", ImageTexture::REPEAT, ImageTexture::NEAREST);
  ImageTexture* bumptexsmooth = new ImageTexture("models/stones_bump.png", ImageTexture::REPEAT, ImageTexture::BILINEAR);
  Camera* cam1 = new PerspectiveCamera(Point(0.048f, 0.043f, -0.050f), Vector(-0.3f, -0.3f, 0.7f), Vector(0, 1, 0), 0.686f, 0.686f);
  //bump(cam1, bumptex, "a8-1a.png");
  //bump(cam1, bumptexsmooth, "a8-1b.png");
  Camera* cam2 = new PerspectiveCamera(Point(0.278f, 0.273f, -0.800f), Vector(0, 0, 1), Vector(0, 1, 0), 0.686f, 0.686f);
  //bump(cam2, bumptexsmooth, "a8-2.png");


  // fog
  PerspectiveCamera cam(Point(16.065f, -12.506f, 1.771f),
    Point(-0.286f, -0.107f, 1.35f) - Point(16.065f, -12.506f, 1.771f),
    Vector(0, 0, 1), pi / 8, pi / 6);
  PerlinTexture* perlin = new PerlinTexture(RGBColor(0.f, 1.f, 0.f), RGBColor::rep(0.f));
  perlin->addOctave(0.5f, 5.0f);
  perlin->addOctave(0.25f, 10.0f);
  perlin->addOctave(0.125f, 20.0f);
  perlin->addOctave(0.125f, 40.0f);
  perlin->addOctave(0.125f, 80.0f);
  perlin->addOctave(0.125f, 160.0f);

  fog(&cam, new UniformFogAmbientLight(RGBColor(0.f, 1.f, 0.f), 0.009f), "a8-3a.png");
  fog(&cam, new UniformFog(RGBColor(0.f, 1.f, 0.f), 0.009f), "a8-3b.png");
  fog(&cam, new HeterogenousFog(perlin, 0.00009f), "a8-3c.png");
}
