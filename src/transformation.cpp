#include "mesh.h"
#include "transformation.h"
#include <vector>
#include <cmath>

Vertex rotateX(const Vertex& v, float angle){
  const auto c = std::cos(angle);
  const auto s = std::sin(angle);

  return {
    v.x, 
    v.y * c - v.z * s, // ycos(theta) - zsin(theta)
    v.y * s + v.z * c // ysin(theta) + zcos(theta)
  };
}

Vertex rotateY(const Vertex& v, float angle){
  const auto c = std::cos(angle);
  const auto s = std::sin(angle);

  return {
    v.x * c + v.z * s, 
    v.y, 
    -v.x * s + v.z * c 
  };
}

Vertex rotateZ(const Vertex& v, float angle){
  const auto c = std::cos(angle);
  const auto s = std::sin(angle);

  return {
    v.x * c - v.y * s, 
    v.x * s + v.y * c,
    v.z 
  };
}

