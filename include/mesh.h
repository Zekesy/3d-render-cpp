#pragma once 
#include <vector>
#include <string>
#include <map>
#include "Vec3.h"
#include "Quadric.h"

struct Vertex {
  Vec3 v; 
  Quadric q; 
};

struct Triangle {
  int v1_index;
  int v2_index;
  int v3_index;
  std::string materialName;

};

struct Material {
  std:: string name;
  float r, g, b; 
};

struct Face {
  std::vector<int> vertices; 
  std:: string materialName;
};

class Mesh {
   //vertices 
  public:
    std::vector<Vertex> vertices; 
    std::vector<Face> faces; //vertex indexes of a face  
    std::vector<Triangle> triangulatedFaces; //vertex indexes of a t
    std::map<std::string, Material> materials;

    
    std::vector<Triangle> triangulateFace(const Face& face);
    void triangulate();
    static Mesh parseMeshFromObj(const std::string obj_path);
  private:
    static void parseMtlFile(const std::string& mtl_path, std::map<std::string, Material>& materials);
};


