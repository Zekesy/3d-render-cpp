#pragma once 
#include <vector>
#include <string>
#include <map>

struct Vertex {
  float x; 
  float y;
  float z;
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


//TODO refactor code to use Face object in faces vector
//TODO Update the mesh.h file to handle the new parsign obj Functions
//TODO Try spinningTop rendering with new functions 

class Mesh {
   //vertices 
  public:
    std::vector<Vertex> vertices; 
    std::vector<Face> faces; //vertex indexes of a face  
    std::vector<Triangle> triangulatedFaces; //vertex indexes of a t
    std::map<std::string, Material> materials;

    
    std::vector<Triangle> triangulateFace(const Face& face);
    void triangulate();
    static Mesh parseMeshFromObj(const std::string obj_path, const std::string mtl_path);
  private:
    static void parseMtlFile(const std::string& mtl_path, std::map<std::string, Material>& materials);
};


