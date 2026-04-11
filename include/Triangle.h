#pragma once

struct Triangle {
  int v0;
  int v1;
  int v2;
  std::string materialName;
  
  bool valid = true;
  Vec3 normal;
};
