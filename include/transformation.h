#pragma once 
#include "mesh.h"

struct Transform {
  float x = 0.0f; 
  float y = 0.0f; 
  float z = 0.0f; 
  float rotX = 0.0f; 
  float rotY = 0.0f; 
  float rotZ = 0.0f;
};

Vertex rotateX(const Vertex& v, float angle);
Vertex rotateY(const Vertex& v, float angle);
Vertex rotateZ(const Vertex& v, float angle);
