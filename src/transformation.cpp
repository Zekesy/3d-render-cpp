#include "transformation.h"
#include <vector>
#include <cmath>

Vec3 rotateX(const Vec3& v, float angle){
  const auto c = std::cos(angle);
  const auto s = std::sin(angle);

  return Vec3(
    v.data[0], 
    v.data[1] * c - v.data[2] * s, // ycos(theta) - zsin(theta)
    v.data[1] * s + v.data[2] * c // ysin(theta) + zcos(theta)
    );
}

Vec3 rotateY(const Vec3& v, float angle){
  const auto c = std::cos(angle);
  const auto s = std::sin(angle);

  return {
    v.data[0] * c + v.data[2] * s, 
    v.data[1], 
    -v.data[0] * s + v.data[2] * c 
  };
}

Vec3 rotateZ(const Vec3& v, float angle){
  const auto c = std::cos(angle);
  const auto s = std::sin(angle);

  return {
    v.data[0] * c - v.data[1] * s, 
    v.data[0] * s + v.data[1] * c,
    v.data[2] 
  };
}


