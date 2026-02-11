//Functions for mesh processing
#include <vector>
#include "mesh.h"

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

