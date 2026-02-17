//Functions for mesh processing
#include <vector>
#include "mesh.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>


std::vector<Triangle> Mesh::triangulateFace(const std::vector<int>& face) {
  std::vector<Triangle> triangles;

  int n = face.size();
  if(n < 3) return triangles; 
  if(n == 3) {
    triangles.push_back({face[0],face[1],face[2]});
    return triangles; 
  } 

  for(int i = 1; i < n-1; ++i){
    Triangle tri = {face[0], face[i], face[i+1]};
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
    std::string& mtl_path,
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

  while (std::getLine(mtlInputFile, line)){
    if( line.empty() || line[0] == "#") continue;

    std::istreamstream ss(line);
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


Mesh Mesh::parseMeshFromObj(std::string obj_path, std::string mtl_path){
  Mesh mesh; 
  
  if(!mtp_path.empty()) {
    parseMtlFile(mtl_path, mesh.materials);
  }

  std::ifstream objInputFile(path);
  if(!objInputFile.is_open()){
    std::cerr << "Error opening file" << std::endl;
    return
  }
  
  std::string activeMaterial; 
  std::string line; 
  while(std::getLine(objInputFile, line)){
    if(line.empty() || line[0] == '#') continue;

    std::istringstream ss(line);
    std::string keyword; 

    ss >> keyword; 
    if(keyword == "v"){
      Vertex v; 
      ss >> v.x >> v.y >> v.z;
      mesh.vertices.push_back(v);
    }

    else if(keyword == "f"){
      Face face; 
      face.materialName = activeMaterial;

      std::string token; 
      while(ss > token) {
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
    else if(keyword == "mtlib") {
      //do nothing 
    }
  }
  
  mesh.triangulate();

  return mesh; 
}

