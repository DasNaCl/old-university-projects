#pragma once

#include <core/scalar.h>
#include <core/color.h>
#include <rt/ray.h>

float a1computeWeight(float fx, float fy, const rt::Point& c, float div);

template <typename T>
rt::RGBColor a1computeColor(T x, T y, T width, T height)
{
  float        fx    = float(x) / float(width);
  float        fy    = float(y) / float(height);
  rt::RGBColor color = rt::RGBColor::rep(0.0f);
  color =
    color + a1computeWeight(fx, fy, rt::Point(-0.8f, 0.156f, 0.0f), 64.0f) *
              rt::RGBColor(0.8f, 0.8f, 1.0f);
  color = color + a1computeWeight(fx, fy, rt::Point(-0.6f, 0.2f, 0.0f), 64.0f) *
                    0.2f * rt::RGBColor(0.5f, 0.5f, -0.2f);
  color =
    color + a1computeWeight(fy, fx, rt::Point(0.285f, 0.0f, 0.0f), 64.0f) *
              rt::RGBColor(0.2f, 0.3f, 0.4f);
  return rt::RGBColor::rep(1.0f) - color;
}

rt::RGBColor a2computeColor(const rt::Ray& r);
