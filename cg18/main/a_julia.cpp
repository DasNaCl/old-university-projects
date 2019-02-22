#include <core/point.h>
#include <core/vector.h>
#include <core/color.h>
#include <core/scalar.h>
#include <core/julia.h>
#include <core/image.h>
#include <rt/renderer.h>

using namespace rt;

float a1computeWeight(float fx, float fy, const Point& c, float div) {
    Point v(fx, fy, 0.5f);
    v = v - Vector::rep(0.5f);
    v = v * 2.0f;
    int numIter = julia(v, c);
    return numIter/(numIter+div);
}

void a_julia() {
    Image img(800, 800);
    Renderer engine(0,0);
    engine.test_render1(img);
    img.writePNG("a1.png");
}
