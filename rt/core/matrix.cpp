#include "assert.h"
#include <core/matrix.h>
#include <core/point.h>
#include <core/vector.h>
namespace rt
{

Matrix Matrix::invert() const
{
  Matrix        result;
  const Matrix& m = *this;

  //Taken and modified from http://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix
  result[0][0] = m[1][1] * m[2][2] * m[3][3] - m[1][1] * m[2][3] * m[3][2] -
                 m[2][1] * m[1][2] * m[3][3] + m[2][1] * m[1][3] * m[3][2] +
                 m[3][1] * m[1][2] * m[2][3] - m[3][1] * m[1][3] * m[2][2];
  result[1][0] = -m[1][0] * m[2][2] * m[3][3] + m[1][0] * m[2][3] * m[3][2] +
                 m[2][0] * m[1][2] * m[3][3] - m[2][0] * m[1][3] * m[3][2] -
                 m[3][0] * m[1][2] * m[2][3] + m[3][0] * m[1][3] * m[2][2];
  result[2][0] = m[1][0] * m[2][1] * m[3][3] - m[1][0] * m[2][3] * m[3][1] -
                 m[2][0] * m[1][1] * m[3][3] + m[2][0] * m[1][3] * m[3][1] +
                 m[3][0] * m[1][1] * m[2][3] - m[3][0] * m[1][3] * m[2][1];
  result[3][0] = -m[1][0] * m[2][1] * m[3][2] + m[1][0] * m[2][2] * m[3][1] +
                 m[2][0] * m[1][1] * m[3][2] - m[2][0] * m[1][2] * m[3][1] -
                 m[3][0] * m[1][1] * m[2][2] + m[3][0] * m[1][2] * m[2][1];

  float det = m[0][0] * result[0][0] + m[0][1] * result[1][0] +
              m[0][2] * result[2][0] + m[0][3] * result[3][0];
  if(det == 0)
    return Matrix::zero();

  result[0][1] = -m[0][1] * m[2][2] * m[3][3] + m[0][1] * m[2][3] * m[3][2] +
                 m[2][1] * m[0][2] * m[3][3] - m[2][1] * m[0][3] * m[3][2] -
                 m[3][1] * m[0][2] * m[2][3] + m[3][1] * m[0][3] * m[2][2];
  result[1][1] = m[0][0] * m[2][2] * m[3][3] - m[0][0] * m[2][3] * m[3][2] -
                 m[2][0] * m[0][2] * m[3][3] + m[2][0] * m[0][3] * m[3][2] +
                 m[3][0] * m[0][2] * m[2][3] - m[3][0] * m[0][3] * m[2][2];
  result[2][1] = -m[0][0] * m[2][1] * m[3][3] + m[0][0] * m[2][3] * m[3][1] +
                 m[2][0] * m[0][1] * m[3][3] - m[2][0] * m[0][3] * m[3][1] -
                 m[3][0] * m[0][1] * m[2][3] + m[3][0] * m[0][3] * m[2][1];
  result[3][1] = m[0][0] * m[2][1] * m[3][2] - m[0][0] * m[2][2] * m[3][1] -
                 m[2][0] * m[0][1] * m[3][2] + m[2][0] * m[0][2] * m[3][1] +
                 m[3][0] * m[0][1] * m[2][2] - m[3][0] * m[0][2] * m[2][1];
  result[0][2] = m[0][1] * m[1][2] * m[3][3] - m[0][1] * m[1][3] * m[3][2] -
                 m[1][1] * m[0][2] * m[3][3] + m[1][1] * m[0][3] * m[3][2] +
                 m[3][1] * m[0][2] * m[1][3] - m[3][1] * m[0][3] * m[1][2];
  result[1][2] = -m[0][0] * m[1][2] * m[3][3] + m[0][0] * m[1][3] * m[3][2] +
                 m[1][0] * m[0][2] * m[3][3] - m[1][0] * m[0][3] * m[3][2] -
                 m[3][0] * m[0][2] * m[1][3] + m[3][0] * m[0][3] * m[1][2];
  result[2][2] = m[0][0] * m[1][1] * m[3][3] - m[0][0] * m[1][3] * m[3][1] -
                 m[1][0] * m[0][1] * m[3][3] + m[1][0] * m[0][3] * m[3][1] +
                 m[3][0] * m[0][1] * m[1][3] - m[3][0] * m[0][3] * m[1][1];
  result[3][2] = -m[0][0] * m[1][1] * m[3][2] + m[0][0] * m[1][2] * m[3][1] +
                 m[1][0] * m[0][1] * m[3][2] - m[1][0] * m[0][2] * m[3][1] -
                 m[3][0] * m[0][1] * m[1][2] + m[3][0] * m[0][2] * m[1][1];
  result[0][3] = -m[0][1] * m[1][2] * m[2][3] + m[0][1] * m[1][3] * m[2][2] +
                 m[1][1] * m[0][2] * m[2][3] - m[1][1] * m[0][3] * m[2][2] -
                 m[2][1] * m[0][2] * m[1][3] + m[2][1] * m[0][3] * m[1][2];
  result[1][3] = m[0][0] * m[1][2] * m[2][3] - m[0][0] * m[1][3] * m[2][2] -
                 m[1][0] * m[0][2] * m[2][3] + m[1][0] * m[0][3] * m[2][2] +
                 m[2][0] * m[0][2] * m[1][3] - m[2][0] * m[0][3] * m[1][2];
  result[2][3] = -m[0][0] * m[1][1] * m[2][3] + m[0][0] * m[1][3] * m[2][1] +
                 m[1][0] * m[0][1] * m[2][3] - m[1][0] * m[0][3] * m[2][1] -
                 m[2][0] * m[0][1] * m[1][3] + m[2][0] * m[0][3] * m[1][1];
  result[3][3] = m[0][0] * m[1][1] * m[2][2] - m[0][0] * m[1][2] * m[2][1] -
                 m[1][0] * m[0][1] * m[2][2] + m[1][0] * m[0][2] * m[2][1] +
                 m[2][0] * m[0][1] * m[1][2] - m[2][0] * m[0][2] * m[1][1];

  result = result * (1.0f / det);
  return result;
}

Matrix Matrix::inverseOf(
  const Float4& r1, const Float4& r2, const Float4& r3, const Float4& r4)
{
  return Matrix(r1, r2, r3, r4).invert();
}

Matrix::Matrix(
  const Float4& r1, const Float4& r2, const Float4& r3, const Float4& r4)
  : rows{ r1, r2, r3, r4 }
{
}

Float4& Matrix::operator[](int idx)
{
  assert(0 <= idx && idx < 4);
  return rows[idx];
}

Float4 Matrix::operator[](int idx) const
{
  assert(0 <= idx && idx < 4);
  return rows[idx];
}
Matrix Matrix::operator+(const Matrix& b) const
{
  return Matrix(rows[0] + b[0], rows[1] + b[1], rows[2] + b[2], rows[3] + b[3]);
}
Matrix Matrix::operator-(const Matrix& b) const {
    return Matrix(rows[0] - b[0], rows[1] - b[1], rows[2] - b[2], rows[3] - b[3]);
}

Matrix Matrix::transpose() const
{
  return Matrix(Float4(rows[0][0], rows[1][0], rows[2][0], rows[3][0]),
    Float4(rows[0][1], rows[1][1], rows[2][1], rows[3][1]),
    Float4(rows[0][2], rows[1][2], rows[2][2], rows[3][2]),
    Float4(rows[0][3], rows[1][3], rows[2][3], rows[3][3]));
}

bool Matrix::operator==(const Matrix& b) const
{
  return std::equal(rows.begin(), rows.end(), b.rows.begin());
}
bool Matrix::operator!=(const Matrix& b) const
{
  return !((*this) == b);
}

Float4 Matrix::operator*(const Float4& b) const
{
  return Float4(
    dot(rows[0], b), dot(rows[1], b), dot(rows[2], b), dot(rows[3], b));
}
Vector Matrix::operator*(const Vector& b) const
{
  Float4 bb(b);
  Float4 res(
    dot(rows[0], bb), dot(rows[1], bb), dot(rows[2], bb), dot(rows[3], bb));
  return Vector(res);
}
Point Matrix::operator*(const Point& b) const
{
  Float4 bb(b);
  Float4 res(
    dot(rows[0], bb), dot(rows[1], bb), dot(rows[2], bb), dot(rows[3], bb));
  return Point(res);
}
float Matrix::det() const
{
  float d1 = rows[1][1] * rows[2][2] * rows[3][3];
  d1 += rows[1][2] * rows[2][3] * rows[3][1];
  d1 += rows[1][3] * rows[2][1] * rows[3][2];
  d1 -= rows[3][1] * rows[2][2] * rows[1][3];
  d1 -= rows[3][2] * rows[2][3] * rows[1][1];
  d1 -= rows[3][3] * rows[2][1] * rows[1][2];
  d1 *= rows[0][0];
  float d2 = rows[0][1] * rows[2][2] * rows[3][3];
  d2 += rows[0][2] * rows[2][3] * rows[3][1];
  d2 += rows[0][3] * rows[2][1] * rows[3][2];
  d2 -= rows[3][1] * rows[2][2] * rows[0][3];
  d2 -= rows[3][2] * rows[2][3] * rows[0][1];
  d2 -= rows[3][3] * rows[2][1] * rows[0][2];
  d2 *= rows[1][0];
  float d3 = rows[0][1] * rows[1][2] * rows[3][3];
  d3 += rows[0][2] * rows[1][3] * rows[3][1];
  d3 += rows[0][3] * rows[1][1] * rows[3][2];
  d3 -= rows[3][1] * rows[1][2] * rows[0][3];
  d3 -= rows[3][2] * rows[1][3] * rows[0][1];
  d3 -= rows[3][3] * rows[1][1] * rows[0][2];
  d3 *= rows[2][0];
  float d4 = rows[0][1] * rows[1][2] * rows[2][3];
  d4 += rows[0][2] * rows[1][3] * rows[2][1];
  d4 += rows[0][3] * rows[1][1] * rows[2][2];
  d4 -= rows[2][1] * rows[1][2] * rows[0][3];
  d4 -= rows[2][2] * rows[1][3] * rows[0][1];
  d4 -= rows[2][3] * rows[1][1] * rows[0][2];
  d4 *= rows[3][0];
  return d1 - d2 + d3 - d4;

}
Matrix Matrix::zero()
{
  Float4 f0(0.f, 0.f, 0.f, 0.f);
  return Matrix(f0, f0, f0, f0);
}
Matrix Matrix::identity()
{
  Float4 f1(1.f, 0.f, 0.f, 0.f);
  Float4 f2(0.f, 1.f, 0.f, 0.f);
  Float4 f3(0.f, 0.f, 1.f, 0.f);
  Float4 f4(0.f, 0.f, 0.f, 1.f);
  return Matrix(f1, f2, f3, f4);
}
Matrix Matrix::system(const Vector& e1, const Vector& e2, const Vector& e3)
{
    Float4 a(e1.x,e2.x,e3.x,0.f);
    Float4 b(e1.y,e2.y,e3.y,0.f);
    Float4 c(e1.z,e2.z,e3.z,0.f);
  return Matrix(a,b,c, Float4(0.f, 0.f, 0.f, 1.f));
}
Matrix product(const Matrix& a, const Matrix& b)
{
  Matrix result = Matrix::zero();
  for(size_t i = 0 ; i< 4 ; ++i)
  {
      for(size_t j = 0; j<4; ++j)
      {
          for(size_t k = 0; k<4; ++k)
          {
              result[i][k] += a[i][j]*b[j][k];
          }
      }
  }
  return result;
}
Matrix operator*(const Matrix& a, float scalar)
{
  return Matrix(a[0] * scalar, a[1] * scalar, a[2] * scalar, a[3] * scalar);
}

Matrix operator*(float scalar, const Matrix& a)
{
  return Matrix(a[0] * scalar, a[1] * scalar, a[2] * scalar, a[3] * scalar);
}
}
