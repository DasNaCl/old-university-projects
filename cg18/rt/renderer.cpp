#include <core/assert.h>
#include <core/color.h>
#include <core/image.h>
#include <core/random.h>
#include <rt/cameras/camera.h>
#include <rt/integrators/integrator.h>
#include <rt/ray.h>
#include <rt/renderer.h>

#include <main/a1.h>
#include <main/scoped_timer.h>

#include <algorithm>
#include <functional>
#include <future>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <thread>
#include <limits>
#include <chrono>
#include <queue>

#ifndef NDEBUG // if we are not in non-debug mode, so if we are debugging....
#define USE_SEQUENTIAL_RENDERER
#endif

namespace rt
{
// can't use inline here, as we have C++11....
static /*inline*/ auto print_results_maker = [](std::ostream&     os,
                                               const std::string& desc) {
  return [&os, desc](std::chrono::milliseconds msecs) {
    os << desc << ": " << std::setw(5)
        << std::chrono::duration_cast<std::chrono::seconds>(msecs).count() << "s\n";
  };
};

Renderer::Renderer(Camera* cam, Integrator* integrator)
  : cam(cam)
  , integrator(integrator)
  , samples(1U)
  , motionBlur(false)
  , time(0.f)
  , totalTime(1.f)
  , framesPerSecond(1.f)
{
}

void Renderer::setSamples(uint samples)
{
  this->samples = samples;
}

void Renderer::setImgName(const std::string& name)
{
  img_name = name;
}

void Renderer::render(Image& img)
{
  auto benchmark_res = print_results_maker(std::cout, img_name);
  ScopedTimer<decltype(benchmark_res)> timer(std::move(benchmark_res));

  /*
  const auto motion_blurry_loop_unparallelized = 
    [&img, this](uint x, uint y) {
      std::size_t totalFrames = 0U;
      RGBColor total0 = RGBColor::rep(0.f);
      //for(time = 0.f; time < totalTime; time += framesPerSecond)
      {
        RGBColor total1 = RGBColor::rep(0.f);
        for(std::size_t i = 0; i < samples; ++i)
        {
          const float sscx = PIDtoSSC(x, img.width());
          const float sscy = PIDtoSSC(y, img.height());

          auto r = cam->getPrimaryRay(sscx, -sscy);
          total1  = total1 + integrator->getRadiance(r);
        }
        total0 = total0 + total1 / static_cast<float>(samples);
        ++totalFrames;
      }
      img(x, y) = total0 / static_cast<float>(totalFrames);
    };
#ifndef USE_SEQUENTIAL_RENDERER
  parallel_tiled_loop(
#else
  sequential_loop(
#endif
    motion_blurry_loop_unparallelized,
    img.width(), img.height());
  */

struct Pixel
{
  const std::size_t x;
  const std::size_t y;
  Renderer& rnd;

  Pixel(Renderer& rnd, std::size_t x, std::size_t y)
    : rnd(rnd), x(x), y(y)
  {  }
  
  void operator()(Image& img)
  {
    RGBColor total = RGBColor::rep(0.f);
    for(std::size_t i = 0; i < rnd.samples; ++i)
    {
      const float sscx = rnd.PIDtoSSC(x, img.width());
      const float sscy = rnd.PIDtoSSC(y, img.height());

      auto r = rnd.cam->getPrimaryRay(sscx, -sscy);
      total = total + rnd.integrator->getRadiance(r);
    }
    // no race condition, as every pixel is written only once
    img(x, y) = total / rnd.samples;
  }
};
    // one Job = 32*32 Pixels; maximal number of jobs: 1024 * 1024
  using PixelQueue = std::vector<Pixel>;
  std::queue<PixelQueue> tasks;
  const std::size_t tile_size = 32;
  const std::size_t max_amount_of_tasks = 1024 * 1024;
  uint old_y0 = 0;
  uint old_x0 = 0;
  bool stuff_to_do = false;

  std::vector<std::chrono::duration<double>> seconds_per_job_storage;

enqueue_tasks: for(uint y0 = old_y0; y0 < img.height(); y0 += tile_size)
  {
    const uint h0 = y0 + tile_size < img.height() ? y0 + tile_size : img.height();
    for(uint x0 = old_x0; x0 < img.width(); x0 += tile_size)
    {
      std::vector<Pixel> pxs;
      const uint w0 = x0 + tile_size < img.width() ? x0 + tile_size : img.width();
      for(uint y = y0; y < h0; ++y)
        for(uint x = x0; x < w0; ++x)
          pxs.emplace_back(*this, x, y);

      tasks.emplace(pxs);
      if(tasks.size() > max_amount_of_tasks)
      {
        old_y0 = y0;
        old_x0 = x0;
        stuff_to_do = true;
        
        // ensure that loop exits
        x0 = y0 = std::numeric_limits<unsigned int>::max();
        break;
      }
    }
  }
  const auto num_cores = std::thread::hardware_concurrency();
  std::vector<std::future<void>> handles;
  handles.reserve(num_cores);
  std::mutex work_mutex;
  for(std::size_t i = 0; i < num_cores; ++i) {
    handles.emplace_back(std::async(std::launch::async,
          [&tasks,&work_mutex,&img,&seconds_per_job_storage]()
          {
            while(!tasks.empty()) {
              auto begin = std::chrono::steady_clock::now();
              PixelQueue* px;
              // take job from queue
              {
              std::lock_guard<std::mutex> lock(work_mutex);
              px = new PixelQueue(tasks.front());
              tasks.pop();
              }
              // run job
              for(auto& p : *px)
                p(img);
              delete px;
              auto end = std::chrono::steady_clock::now();

              //track time
              {
              std::lock_guard<std::mutex> lock(work_mutex);
              seconds_per_job_storage.push_back(std::chrono::duration_cast<std::chrono::seconds>(end - begin));
              }
            }
          }));
  }
  while(!handles.empty())
  {
    for(auto hit = handles.begin(); hit != handles.end(); ) {
      auto& h = *hit;
      /*
#ifndef NDEBUG
      h.get(); // make no printouts in release mode -> be as fast as possible
      hit = handles.erase(hit);
#else
*/
      auto status = h.wait_for(std::chrono::milliseconds(500));
      if(status == std::future_status::ready) {
        handles.erase(hit);
      }
      else {
        ++hit;
      }
//#endif
    }
//#ifndef NDEBUG
    if(tasks.size() > 0)
    {
      std::lock_guard<std::mutex> lock(work_mutex);
      const float total_seconds = std::accumulate(seconds_per_job_storage.begin(),
                                                  seconds_per_job_storage.end(), 0.f,
                                  [](float acc, const std::chrono::duration<float>& dt)
                                  { return acc + dt.count(); });

      const float estimated_seconds_per_job = total_seconds / seconds_per_job_storage.size();
      const float estimation = (estimated_seconds_per_job * tasks.size());
      std::cerr << handles.size() << " agents are working on " << tasks.size() << " tasks. [so ~"
                    << (tasks.size() * tile_size * tile_size) << " pixel are left]\t"
                    << "ESTIMATION ";

      const float hours = estimation / (60.f * 60.f);
      const float minutes = estimation / (60.f);
      const float seconds = estimation;

      if(seconds < 30.f)
        std::cerr << seconds << "s\n";
      else if(minutes < 30.f)
        std::cerr << (estimation / 60.f) << "min\n";
      else
        std::cerr << (estimation / 60.f * 60.f) << "h\n";
    }
//#endif
  }
  if(stuff_to_do)
  {
    seconds_per_job_storage.clear();
    stuff_to_do = false;
    goto enqueue_tasks;
  }
}

void Renderer::test_render1(Image& img)
{
  //  auto benchmark_res = print_results_maker(std::cout, "test_render1");
  //  ScopedTimer<decltype(benchmark_res)> timer(std::move(benchmark_res));

  parallel_tiled_loop(
    [&img](uint x, uint y) {
      img(x, y) = a1computeColor(x, y, img.width(), img.height());
    },
    img.width(), img.height());
}

void Renderer::test_render2(Image& img)
{
  //  auto benchmark_res = print_results_maker(std::cout, "test_render2");
  //  ScopedTimer<decltype(benchmark_res)> timer(std::move(benchmark_res));

  parallel_tiled_loop(
    [&img, this](uint x, uint y) {
      const float sscx = PIDtoSSC(x, img.width());
      const float sscy = PIDtoSSC(y, img.height());

      auto r = cam->getPrimaryRay(sscx, -sscy);

      img(x, y) = a2computeColor(r);
    },
    img.width(), img.height());
}

void Renderer::setFramesPerSecondAndTotalTime(float val0, float val1)
{
  framesPerSecond = val0;
  totalTime       = val1;
}

template <typename T>
void Renderer::sequential_loop(T&& func, uint w, uint h)
{
  for(uint y = 0; y < h; ++y)
    for(uint x = 0; x < w; ++x)
      func(x, y);
}

template <typename T>
void Renderer::tiled_loop(T&& func, uint w, uint h)
{
  for(uint y0 = 0; y0 < h; y0 += batch_size)
  {
    const uint h0 = y0 + batch_size < h ? y0 + batch_size : h;
    for(uint x0 = 0; x0 < w; x0 += batch_size)
    {
      const uint w0 = x0 + batch_size < w ? x0 + batch_size : w;
      for(uint y = y0; y < h0; ++y)
      {
        for(uint x = x0; x < w0; ++x)
        {
          func(x, y);
        }
      }
    }
  }
}

template <typename T>
void Renderer::parallel_tiled_loop(T&& func, uint w, uint h)
{
  const auto num_cores = std::thread::hardware_concurrency();

  assert(batch_size > 0);
  const bool over_width =
    std::max(w / batch_size, h / batch_size) == (w / batch_size);
  const std::size_t total_blocks = std::max(w / batch_size, h / batch_size) + 1;
  if(total_blocks < num_cores)
  {
    tiled_loop(std::forward<T&&>(func), w, h);
    return;
  }

  assert(num_cores > 0);
  const std::size_t block_offset =
    (over_width ? total_blocks * batch_size != w
                : total_blocks * batch_size != h);
  const std::size_t blocks_per_thread = total_blocks / num_cores + block_offset;

  const uint                     sampleCount = samples;
  std::vector<std::future<void>> handles;
  handles.reserve(num_cores);
  for(std::size_t i = 0U; i < num_cores; ++i)
  {
    const uint xs = (over_width ? i * blocks_per_thread * batch_size : 0);
    const uint xe = (over_width ? (i + 1) * blocks_per_thread * batch_size : w);

    const uint ys = (!over_width ? i * blocks_per_thread * batch_size : 0);
    const uint ye =
      (!over_width ? (i + 1) * blocks_per_thread * batch_size : h);

    handles.emplace_back(std::async(std::launch::async,
      [&func, w, h, sampleCount](
        uint startx, uint endx, uint starty, uint endy) {
        for(uint y0 = starty; y0 < endy; y0 += batch_size)
        {
          const uint h0 = y0 + batch_size < h ? y0 + batch_size : h;
          for(uint x0 = startx; x0 < endx; x0 += batch_size)
          {
            const uint w0 = x0 + batch_size < w ? x0 + batch_size : w;
            for(uint y = y0; y < h0; ++y)
            {
              for(uint x = x0; x < w0; ++x)
              {
                func(x, y);
              }
            }
          }
        }
      },
      xs, xe, ys, ye));
  }
  for(auto& h : handles)
    h.get();
}

float Renderer::PIDtoSSC(float pid, float res)
{
  const float prc = pid + (samples > 1U ? random(0.f, 1.f) : 0.5f);
  const float ndc = prc / res;
  return ndc * 2 - 1;
}

void Renderer::shouldRenderMotion()
{
  motionBlur = true;
}

float Renderer::getCurrentTimeStep()
{
  return time;
}

float Renderer::getMoveDuration()
{
  return totalTime;
}
}
