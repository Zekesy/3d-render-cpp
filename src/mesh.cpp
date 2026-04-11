//Functions for mesh processing
#include <vector>
#include <set>
#include "mesh.h"
#include "Edge.h"
#include "Triangle.h"
#include "Vertex.h"
#include "Face.h"
#include <queue>                                                                             
#include "Quadric.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <climits>
#include <algorithm>

std::vector<Triangle> Mesh::triangulateFace(const Face& f) {
  std::vector<Triangle> triangles;
  const auto& face = f.vertices; 

  int n = face.size();
  if(n < 3) return triangles; 

  for(int i = 1; i < n-1; ++i){
    Triangle tri;
    tri.v0 = face[0];
    tri.v1 = face[i];      
    tri.v2 = face[i+1];
    tri.materialName = f.materialName; 

    triangulatedFaces.push_back(tri);
    
    edges.push_back(Edge(tri.v0, tri.v1));
    edges.push_back(Edge(tri.v1, tri.v2));
    edges.push_back(Edge(tri.v2, tri.v0));

  }

  return triangles; 
}

void Mesh::triangulate() {
  triangulatedFaces.clear();
  edges.clear();
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

// ----------------- Connectivity --------------------------------
void Mesh::buildConnectivity() {
   for(auto& v: vertices){
    v.connectedVertices.clear();
    v.connectedFaces.clear();
  }
    for (int i = 0; i < triangulatedFaces.size(); i++) {
        auto& t = triangulatedFaces[i];
        if(!t.valid) continue;
        vertices[t.v0].connectedVertices.insert(t.v1);
        vertices[t.v0].connectedVertices.insert(t.v2);

        vertices[t.v1].connectedVertices.insert(t.v0);
        vertices[t.v1].connectedVertices.insert(t.v2);

        vertices[t.v2].connectedVertices.insert(t.v0);
        vertices[t.v2].connectedVertices.insert(t.v1);

        vertices[t.v0].connectedFaces.insert(i);
        vertices[t.v1].connectedFaces.insert(i);
        vertices[t.v2].connectedFaces.insert(i);
    }
}

std::set<int> Mesh::getConnectedVertices(int v) {
    return vertices[v].connectedVertices;
}



Vec3 Mesh::computeNormal(const Triangle& t) {

    Vec3 a = vertices[t.v0].v;
    Vec3 b = vertices[t.v1].v;
    Vec3 c = vertices[t.v2].v;

    return (b - a).cross(c - a).normalized();
}

bool Mesh::triangleFlipCheck(int v1, int v2, const Vec3& newPos) {

    std::set<int> faces;

    // collect all faces connected to v1 and v2
    faces.insert(vertices[v1].connectedFaces.begin(),
                 vertices[v1].connectedFaces.end());

    faces.insert(vertices[v2].connectedFaces.begin(),
                 vertices[v2].connectedFaces.end());

    for(int faceIndex : faces) {

        Triangle& tri = triangulatedFaces[faceIndex];
        if(!tri.valid) continue;

        // skip triangulatedFacesthat will disappear
        if(
            (tri.v0 == v1 && tri.v1 == v2) ||
            (tri.v1 == v1 && tri.v2 == v2) ||
            (tri.v2 == v1 && tri.v0 == v2) ||
            (tri.v0 == v2 && tri.v1 == v1) ||
            (tri.v1 == v2 && tri.v2 == v1) ||
            (tri.v2 == v2 && tri.v0 == v1)
        ) continue;

        // old normal
        Vec3 oldNormal = computeNormal(tri);

        // simulate collapse
        Triangle newTri = tri;

        if(newTri.v0 == v2) newTri.v0 = v1;
        if(newTri.v1 == v2) newTri.v1 = v1;
        if(newTri.v2 == v2) newTri.v2 = v1;

        // temporarily move vertex
        Vec3 oldPos = vertices[v1].v;
        vertices[v1].v = newPos;

        Vec3 newNormal = computeNormal(newTri);

        // restore position
        vertices[v1].v = oldPos;

        if(oldNormal.dot(newNormal) < 0.0f)
            return false;
    }

    return true;
}
bool Mesh::hasMoreThanTwoNeighbors(int v1, int v2) {

    auto& n1 = vertices[v1].connectedVertices;
    auto& n2 = vertices[v2].connectedVertices;

    int common = 0;

    for(auto v : n1) {
        if(n2.count(v)) {
            common++;
            if(common > 2) return true;
        }
    }

    return false;
}

bool Mesh::isBoundaryEdge(int v1, int v2) {
    int count = 0;

    for(auto faceIndex : vertices[v1].connectedFaces) {
        auto& t = triangulatedFaces[faceIndex];

        if(
            (t.v0 == v1 && t.v1 == v2) ||
            (t.v1 == v1 && t.v2 == v2) ||
            (t.v2 == v1 && t.v0 == v2) ||
            (t.v0 == v2 && t.v1 == v1) ||
            (t.v1 == v2 && t.v2 == v1) ||
            (t.v2 == v2 && t.v0 == v1)
        ) {
            count++;
        }
    }

    return count == 1;
}
// -------------------- QEM

void Mesh::replaceVertex(int oldV, int newV){
  for(auto& tri: triangulatedFaces) {
    if(tri.v0 == oldV) tri.v0 = newV;
    if(tri.v1 == oldV) tri.v1 = newV;
    if(tri.v2 == oldV) tri.v2 = newV;
  }
}


void Mesh::removeDegenerateTriangles(){
  for(auto& t : triangulatedFaces){
    if(t.v0 == t.v1 || t.v1 == t.v2 || t.v2 == t.v0){
      t.valid = false;
    }
  }
}

void Mesh::collapseEdge(Edge& edge){
  Vec3 newPos = edge.collapsePosition; 
  vertices[edge.v1].v = newPos;
  
  vertices[edge.v1].q = vertices[edge.v1].q + vertices[edge.v2].q;

  replaceVertex(edge.v2, edge.v1);
  removeDegenerateTriangles();
  vertices[edge.v2].valid = false;
}

struct EdgeCompare {
  bool operator()(const Edge& a, const Edge& b) const {
    return a.error > b.error;
  }
};

void Mesh::computeEdgeCost(Edge& edge) {

    Vertex& v1 = vertices[edge.v1];
    Vertex& v2 = vertices[edge.v2];

    Quadric Q = v1.q + v2.q;

    Vec3 best;
    bool valid = Q.solve(best);

    if(!valid) {
        Vec3 mid = (v1.v + v2.v) * 0.5;

        float e1 = Q.evaluate(v1.v);
        float e2 = Q.evaluate(v2.v);
        float e3 = Q.evaluate(mid);

        if(e1 < e2 && e1 < e3)
            best = v1.v;
        else if(e2 < e3)
            best = v2.v;
        else
            best = mid;
    }

    edge.collapsePosition = best;
    edge.error = Q.evaluate(best);
}
void Mesh::applyQEM(int target_count) {
  std::cout << "Build Connectivity" << std::endl;
  buildConnectivity();
  //Compute Quadric 
  for(const auto& tri : triangulatedFaces){
    Quadric::quadricTriangle(vertices[tri.v0], vertices[tri.v1], vertices[tri.v2]);
  }

  std::cout << "Calculated Quadric triangles" << std::endl;
  //compute edge error for unique edges 
  std::set<std::pair<int,int>> edgeSet; 
  
  //find unique edges 
  // for(auto& edge : model.edges){
  //   int a = std::min(edge.v1, edge.v2);
  //   int b = std::max(edge.v1, edge.v2);
  //   edgeSet.insert({a,b});
  // }

  std::priority_queue<Edge, std::vector<Edge>, EdgeCompare> pq;

  for(auto& edge: edges) {
      // int a = std::min(edge.v1, edge.v2);
      // int b = std::max(edge.v1, edge.v2);
      //
      // if(edgeSet.find({a,b}) != edgeSet.end()) {
    computeEdgeCost(edge); 
    pq.push(edge);                         
      // }
  }
  
  std::cout << "Computed edge costs and made pq" << std::endl;

  int validCount = triangulatedFaces.size();
  while(!pq.empty() && validCount > target_count){
    Edge edge = pq.top();
    pq.pop();

    //Checks 
    if(hasMoreThanTwoNeighbors(edge.v1, edge.v2)) continue;
    if(isBoundaryEdge(edge.v1, edge.v2)) continue;
    if(!triangleFlipCheck(edge.v1, edge.v2, edge.collapsePosition)) continue;

    std::cout << "Checks finished" << std::endl;
    //collapse edge 
    collapseEdge(edge);
    auto& neighbors = vertices[edge.v1].connectedVertices;
    
    std::cout << "Collapsed Edge" << std::endl;
    for(int v : neighbors){
        Edge newEdge(edge.v1, v);
        computeEdgeCost(newEdge);
        pq.push(newEdge);
    }
    validCount = 0;
    for(const auto& t : triangulatedFaces){
        if(t.valid) validCount++;
    }
    std::cout << "Invalid Triangles: " << validCount << std::endl;
  }
}

