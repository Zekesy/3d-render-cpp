#include "Vec3.h"

Vec3 Vec3::operator-(const Vec3& other) const {
    return Vec3(
        data[0] - other.data[0],
        data[1] - other.data[1],
        data[2] - other.data[2]
    );
}

Vec3 Vec3::normalized() const {
  float lengthInverse = 1 / length();
  return Vec3 (
    data[0] * lengthInverse,
    data[1] * lengthInverse,
    data[2] * lengthInverse); 
}

float Vec3::length() const { 
  return sqrt(magnitude());
}

float Vec3::magnitude() const {
  float ret = 0.0;
  for(int index =0; index < 3; index++){
    ret += data[index] * data[index];
  }
  return ret;
}

float Vec3::dot(const Vec3& a, const Vec3& b)  {
  return a[0] * b[0] +
         a[1] * b[1] + 
         a[2] * b[2];
}

Vec3 Vec3::cross(const Vec3& other){
    return Vector3(
		data[1]*other.data[2] - data[2]*other.data[1] ,
		data[2]*other.data[0] - data[0]*other.data[2] ,
		data[0]*other.data[1] - data[1]*other.data[0] );
}
