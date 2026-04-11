#pragma once 

struct Vertex {
  Vec3 v; 
  Quadric q; 
  std::set<int> connectedVertices;
  std::set<int> connectedFaces;

  bool valid = true;
};
