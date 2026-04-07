#pragma once 
#include "Vec3.h"

struct Transform {
  float x = 0.0f; 
  float y = 0.0f; 
  float z = 0.0f; 
  float rotX = 0.0f; 
  float rotY = 0.0f; 
  float rotZ = 0.0f;

  static Vec3 rotateX(const Vec3& v, float angle);
  static Vec3 rotateY(const Vec3& v, float angle);
  static Vec3 rotateZ(const Vec3& v, float angle);
};

