#pragma once 
#include <vector>

struct Vertex {
  float x; 
  float y;
  float z;
};

struct Triangle {
  int v1_index;
  int v2_index;
  int v3_index;
};

class Mesh {
   //vertices 
  public:
    std::vector<Vertex> vertices; 
    std::vector<std::vector<int>> faces; //vertex indexes of a face  
    std::vector<Triangle> triangulatedFaces; //vertex indexes of a t

    std::vector<Triangle> triangulateFace(const std::vector<int>& face);
    void triangulate();
};


