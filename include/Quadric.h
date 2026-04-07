#pragma once
#include "Vec3.h"

struct Quadric {
  float Q[4][4];

  Quadric();
  static Quadric fromTriangle(const Vec3& v1, const Vec3& v2, const Vec3& v3);
  void add(Quadric& other);
}
