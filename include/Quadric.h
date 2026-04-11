#pragma once
#include "Vec3.h"

struct Vertex; //forward declaration - Vertex is defined in mesh.h 

struct Quadric {
  float Q[4][4];

  Quadric() {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            Q[i][j] = 0.0f;
  }
  Quadric operator+(const Quadric& other) const {
      Quadric result;
      for (int i = 0; i < 4; i++)
          for (int j = 0; j < 4; j++)
              result.Q[i][j] = Q[i][j] + other.Q[i][j];
      return result;
  }

  Quadric& operator+=(const Quadric& other) {
      for (int i = 0; i < 4; i++)
          for (int j = 0; j < 4; j++)
              Q[i][j] += other.Q[i][j];
      return *this;
  }
  
  Quadric operator*(float x) const {
    Quadric result; 
    for (int i = 0; i < 4; i++){
      for (int j = 0; j < 4; j++) {
        result.Q[i][j] = Q[i][j] * x;
      }
    }
    return result; 
  }
  static Quadric quadric(const Vec3& v1, const Vec3& v2, const Vec3& v3);
  float evaluate(const Vec3& v) const;
  bool solve(Vec3& result) const;
  static void quadricTriangle(Vertex& v0, Vertex& v1,  Vertex& v2);
  static float computeError(const Vec3& v, const Quadric& Q);
};
