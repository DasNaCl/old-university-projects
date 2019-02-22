#ifndef CG1RAYTRACER_RENDERER_HEADER
#define CG1RAYTRACER_RENDERER_HEADER

#include <core/scalar.h>

namespace rt {

class Image;
class Camera;
class Integrator;

class Renderer {
  friend class Motion;

public:
    constexpr static const uint batch_size = 32;
public:
  Renderer(Camera* cam, Integrator* integrator);
  void setSamples(uint samples);
  void setImgName(const std::string& name);
  void render(Image& img);
  void test_render1(Image& img);
  void test_render2(Image& img);

  void setFramesPerSecondAndTotalTime(float fps, float time);

private:
  template <typename T>
  void sequential_loop(T&& func, uint w, uint h);

  template <typename T>
  void tiled_loop(T&& func, uint w, uint h);

  template <typename T>
  void parallel_tiled_loop(T&& func, uint w, uint h);

  float PIDtoSSC(float pid, float res);

  void  shouldRenderMotion();
  float getCurrentTimeStep();
  float getMoveDuration();

private:
  Camera*     cam;
  Integrator* integrator;
  uint        samples;
  std::string img_name;

  bool  motionBlur;
  float time;
  float totalTime;

  float framesPerSecond;
};

}

#endif
