#pragma once

#include <chrono>
#include <iostream>

/** Usage:
 *  
 *  {
 *  auto f = [](std::chrono::milliseconds x)
 *  {
 *    // do something with x
 *  };
 *  
 *  // Sadly, since we use C++11, we must do it like
 *  // this. In C++17, this would be more concise:
 *  //  ScopedTimer tim(std::move(f));
 *  // Note that the lambda could also be constexpr-ified in C++17
 *
 *  ScopedTimer<decltype(f)> tim(std::move(f));
 *
 *  // Now do the stuff that should be timed
 *
 *  }
 * */
template <class F>
class ScopedTimer : public F
{
public:
  using TimePointT = std::chrono::time_point<std::chrono::steady_clock>;

public:
  ScopedTimer(F&& f)
    : F(std::forward<F&&>(f))
    , start(std::chrono::steady_clock::now())
    , end()
  {
  }
  ~ScopedTimer()
  {
    end = std::chrono::steady_clock::now();

    (*this)(std::chrono::duration_cast<std::chrono::milliseconds>(end - start));
  }

  using F::operator();

private:
  TimePointT start;
  TimePointT end;
};
