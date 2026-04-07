#include "Quadric.h""
#include "Vec3.h"


static Quadric Quadric::fromTriangle(const Vec3& v0, const Vec3& v1, const Vec3& v2) {
  Vec3 e1 = v1 - v0;
  Vec3 e2 = v2 - v0;
  Vec3 n = e1.crossProduct(e2).Normalized();

  float d = -n.dot(v0);

  Quadric q; 

  float p = {n.data[0], n.data[1], n.data[2], d};

  for(int i = 0; i < 4; i++){
    for(int j = 0; j < 4; j++){
      q.Q[i][j] = p[i] * p[j]; 
    }
  }

  return q; 
}
