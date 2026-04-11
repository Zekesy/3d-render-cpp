#include "Quadric.h"
#include "Vec3.h"
#include "mesh.h"
#include <cmath>
float Quadric::computeError(const Vec3& v, const Quadric& Q)
{
    float x = v.data[0];
    float y = v.data[1];
    float z = v.data[2];

    return 
        Q.Q[0][0]*x*x +
        Q.Q[0][1]*x*y +
        Q.Q[0][2]*x*z +
        Q.Q[0][3]*x +

        Q.Q[1][0]*y*x +
        Q.Q[1][1]*y*y +
        Q.Q[1][2]*y*z +
        Q.Q[1][3]*y +

        Q.Q[2][0]*z*x +
        Q.Q[2][1]*z*y +
        Q.Q[2][2]*z*z +
        Q.Q[2][3]*z +

        Q.Q[3][0]*x +
        Q.Q[3][1]*y +
        Q.Q[3][2]*z +
        Q.Q[3][3];
}

float Quadric::evaluate(const Vec3& v) const {
    float x = v.data[0];
    float y = v.data[1];
    float z = v.data[2];

    return
        Q[0][0]*x*x +
        2*Q[0][1]*x*y +
        2*Q[0][2]*x*z +
        2*Q[0][3]*x +

        Q[1][1]*y*y +
        2*Q[1][2]*y*z +
        2*Q[1][3]*y +

        Q[2][2]*z*z +
        2*Q[2][3]*z +

        Q[3][3];
}

bool Quadric::solve(Vec3& result) const {
    float a00 = Q[0][0];
    float a01 = Q[0][1];
    float a02 = Q[0][2];

    float a10 = Q[1][0];
    float a11 = Q[1][1];
    float a12 = Q[1][2];

    float a20 = Q[2][0];
    float a21 = Q[2][1];
    float a22 = Q[2][2];

    float b0 = -Q[0][3];
    float b1 = -Q[1][3];
    float b2 = -Q[2][3];

    float det =
        a00 * (a11 * a22 - a12 * a21) -
        a01 * (a10 * a22 - a12 * a20) +
        a02 * (a10 * a21 - a11 * a20);

    if (fabs(det) < 1e-10f)
        return false;

    float invDet = 1.0f / det;

    float x =
        invDet * (
            b0 * (a11 * a22 - a12 * a21) -
            a01 * (b1 * a22 - a12 * b2) +
            a02 * (b1 * a21 - a11 * b2)
        );

    float y =
        invDet * (
            a00 * (b1 * a22 - a12 * b2) -
            b0 * (a10 * a22 - a12 * a20) +
            a02 * (a10 * b2 - b1 * a20)
        );

    float z =
        invDet * (
            a00 * (a11 * b2 - b1 * a21) -
            a01 * (a10 * b2 - b1 * a20) +
            b0 * (a10 * a21 - a11 * a20)
        );

    result = Vec3(x, y, z);
    return true;
}

Quadric Quadric::quadric(const Vec3& v0, const Vec3& v1, const Vec3& v2) {
  Vec3 e1 = v1 - v0;
  Vec3 e2 = v2 - v0;
  Vec3 n = e1.cross(e2).normalized();

  float d = -1.0f * n.dot(v0);

  Quadric q; 

  float p[4] = {n.data[0], n.data[1], n.data[2], d};
  

  for(int i = 0; i < 4; i++){
    for(int j = 0; j < 4; j++){
      q.Q[i][j] = p[i] * p[j]; 
    }
  }
  
  
  return q; 
}


void Quadric::quadricTriangle(Vertex& v0, Vertex& v1,  Vertex& v2) {
  Quadric q = quadric(v0.v, v1.v, v2.v); 
  
  v0.q += q; 
  v1.q += q; 
  v2.q += q; 
}


