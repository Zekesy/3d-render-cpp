//Functions for mesh processing
#include <vector>
#include "mesh.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>


std::vector<Triangle> Mesh::triangulateFace(const Face& f) {
  std::vector<Triangle> triangles;
  const auto& face = f.vertices; 

  int n = face.size();
  if(n < 3) return triangles;  

  for(int i = 1; i < n-1; ++i){
    Triangle tri;
    tri.v1_index = face[0];
    tri.v2_index = face[i];
    tri.v3_index = face[i+1];
    tri.materialName = f.materialName;
    triangles.push_back(tri);
  }

  return triangles; 
}

void Mesh::triangulate() {
  triangulatedFaces.clear();
  int n = faces.size();
  for(const auto& face : faces){
    std::vector<Triangle> faceTriangles = triangulateFace(face);
    
    triangulatedFaces.insert( // Insert all elements in [first,last) before pos 
      triangulatedFaces.end(), //position
      faceTriangles.begin(), //first element
      faceTriangles.end() //one past last 
    );
  } 
}



void Mesh::parseMtlFile(
    const std::string& mtl_path,
    std::map<std::string, Material>& materials
) {
  std::ifstream mtlInputFile(mtl_path);
  if(!mtlInputFile.is_open()){
    std::cerr << "Error opening mtl file" << std::endl;
    return;
  }

  std::string line;
  Material current;
  bool hasMaterial = false; 

  while (std::getline(mtlInputFile, line)){
    if( line.empty() || line[0] == '#') continue;

    std::istringstream ss(line);
    std::string keyword; 
    ss >> keyword; 

    if(keyword == "newmtl"){
      if (hasMaterial) {
        //save it
        materials[current.name] = current;
      }
      ss >> current.name;
      current.r = current.g = current.b = 1.0f; //default white 
      hasMaterial = true;
    }
    else if (keyword == "Kd") {
      ss >> current.r >> current.g >> current.b;
    }
    //can add ka ks later 
  }

  if(hasMaterial){
    materials[current.name] = current; 
  }

  return; 
}


Mesh Mesh::parseMeshFromObj(const std::string obj_path){
  Mesh mesh; 
  std::string mtl_path = obj_path; 
  mtl_path.replace(mtl_path.find_last_of('.'), std::string::npos, ".mtl");
  
  std::ifstream test(mtl_path);
  if(test.is_open()){
    parseMtlFile(mtl_path, mesh.materials);
  }

  std::ifstream objInputFile(obj_path);
  if(!objInputFile.is_open()){
    std::cerr << "Error opening file" << std::endl;
    return mesh;
  }
  
  std::string activeMaterial; 
  std::string line; 
  while(std::getline(objInputFile, line)){
    if(line.empty() || line[0] == '#') continue;

    std::istringstream ss(line);
    std::string keyword; 

    ss >> keyword; 
    if(keyword == "v"){
      Vertex v;
      float x, y, z;
      ss >> x >> y >> z;
      v.v = Vec3(x,y,z);
      mesh.vertices.push_back(v);
    }

    else if(keyword == "f"){
      Face face; 
      face.materialName = activeMaterial;

      std::string token; 
      while(ss >> token) {
        int vertexIndex = std::stoi(token.substr(0, token.find('/')));

        if(vertexIndex > 0){
          face.vertices.push_back(vertexIndex - 1);
        } else if(vertexIndex < 0){
          face.vertices.push_back(
              static_cast<int>(mesh.vertices.size()) + vertexIndex
          );
        }
        
      }
      if(face.vertices.size() >= 3){
        mesh.faces.push_back(face);
      }
    }
    else if(keyword == "usemtl"){
      ss >> activeMaterial;
    }
    else if(keyword == "mtllib") {
      //do nothing 
    }
  }
  
  mesh.triangulate();

  return mesh; 
}

