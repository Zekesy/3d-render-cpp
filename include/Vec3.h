#pragma once 

#include <iostream>
#include <math.h>

class Vec3 {
public:
  float data[3];
  Vec3(float x, float y, float z): data {x,y,z} {}
  Vec3(float val): data{val,val,val} {}
  Vec3 (): data{0,0,0} {}
  
  Vec3 normalized() const;
  float length() const; 
  float magnitude() const; 
 
  Vec3 dot(const Vec3& a, const Vec3& b); 
  Vec3 cross(const Vec3& other);

  Vec3 operator-(const Vec3& other) const;
};
