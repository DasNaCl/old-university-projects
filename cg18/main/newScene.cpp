

#include <core/assert.h>
#include <core/image.h>
#include <core/scalar.h>
#include <rt/renderer.h>
#include <rt/world.h>

#include <cmath>

#include "scoped_timer.h"
#include <chrono>
#include <rt/cameras/perspective.h>
#include <rt/groups/bvh.h>
#include <rt/groups/simplegroup.h>
#include <rt/integrators/casting.h>
#include <rt/integrators/recraymarching.h>
#include <rt/integrators/volumetracer.h>
#include <rt/lights/pointlight.h>
#include <rt/loaders/obj.h>
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
#include <rt/integrators/recraytrace.h>
#include <rt/cameras/dofperspective.h>
#include <rt/lights/projectivecolorlight.h>
#include <rt/integrators/raytrace.h>
#include <rt/materials/fuzzymirror.h>
#include <rt/materials/phong.h>

using namespace rt;

void glass(std::string& file){

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
    auto fog = new HeterogenousFog(perlin, 0.09f);

    Image img(400, 300);
    Image img2(800, 600);

    auto Pos = Point(3,0.3f,0);

    auto focDist =  (5*(Pos + Vector(0,0,-1))-Point( 3, .5f, 1)).length() + 0.42f;

    PerspectiveCamera cam(4*Pos,{-1,0.f,-0.f},{ 0,4,0},pi / 8, pi / 6);
    PerspectiveCamera cam2(5*(Pos + Vector(0,0,-1)),{-1,0.f,0.4f},{ 0,4,0},pi / 8, pi / 6);
    DOFPerspectiveCamera dofCam(5*(Pos + Vector(0,0,-1)),{-1,0.f,0.4f},{ 0,4,0},pi / 8, pi / 6, focDist, 1);
    World world;

    BVH* scene = new BVH();
#define MODELS
#ifdef MODELS
    loadOBJ(scene, "models/", file);
#endif

    Texture*  blacktex = new ConstantTexture(RGBColor(0.f, 0.f, 0.f));
    Texture*  whitetex = new ConstantTexture(RGBColor(1.f, 1.f, 1.f));
    Material* material = new LambertianMaterial(blacktex, whitetex);
    Material* mirror = new FuzzyMirrorMaterial(2.485f, 3.433f, 0.05f);
    Material* phong = new PhongMaterial(whitetex, 10.0f);
    Material* glass = new GlassMaterial(2.0f);
    world.scene = scene;
    world.volume = fog;
    scene->setMaterial(material);

    scene->add(new InfinitePlane({0,0,0},{0,1,0}, nullptr,mirror));

    scene->add(new Sphere(Point( 3, .5f, 1), .5, nullptr, glass));
    scene->add(new Sphere(Point(4.5f, .6f, 1.5f), .6, nullptr, glass));
    scene->add(new Sphere(Point( 2, .5f, -2), .3, nullptr, glass));

    //world.light.push_back(
      //      new PointLight(Point(4.07625,  1.00545, 3.90386), RGBColor(92 * 0.5f,89 * 0.5f,0)));
            //new PointLight(Point(4.07625,  1.00545, 5.90386), RGBColor(92,89,0)));
    world.light.push_back(
            new ProjectiveColorLight(Point(0,6,0),{0,-1,0}));
    
     world.light.push_back(
            new PointLight(Point(4,  1.00545, -3.90386), RGBColor(92 * 0.5f,89 * 0.5f,0)));
    //new PointLight(Point(4.07625,  2.00545, -24.90386), RGBColor(1000,1000,1000)));
    RecursiveRayTracingIntegrator integrator2(&world);
    RecRayMarching integrator1(&world, true);

    Renderer       engine(&cam2 , &integrator2);
    Renderer       engine2(&dofCam, &integrator2);
    Renderer       engine3(&cam2, &integrator1);
    Renderer       engine4(&dofCam, &integrator1);
    Renderer       engine5(&cam, &integrator1);

    engine.setSamples(10);
    engine2.setSamples(10);
    engine3.setSamples(10);
    engine4.setSamples(10);
    engine5.setSamples(10);

    scene->rebuildIndex();
    engine.render(img2);
    auto s = file + "_1.png";
    img2.writePNG(s);
    std::cout << s <<std::endl;

    s = file + "_2.png";
    engine2.render(img2);
    img2.writePNG(s);
    std::cout << s <<std::endl;

    s = file + "_3.png";
    engine3.render(img2);
    img2.writePNG(s);
    std::cout << s <<std::endl;

    s = file + "_4.png";
    engine4.render(img2);
    img2.writePNG(s);
    std::cout << s <<std::endl;

    s = file + "_5.png";
    engine5.render(img2);
    img2.writePNG(s);
    std::cout << s <<std::endl;

}


void run(){
    
   std::string s1 = "house.obj"; 
   std::string s2 = "rex2.obj";
   std::string s3 = "re.obj";
   
   glass(s1);
   glass(s2);
   glass(s3);
}
