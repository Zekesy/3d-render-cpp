#pragma once 
#include <vector>
#include <string>
#include <set>
#include <map>
#include "Vec3.h"
#include "Quadric.h"
#include "Face.h"
#include "Triangle.h"
#include "Edge.h"
#include "Vertex.h"
#include <set>


struct Material {
  std:: string name;
  float r, g, b; 
};

class Mesh {
   //vertices 
  public:
    std::vector<Vertex> vertices;
    std::vector<Vertex> QEMVertices; 
    std::vector<Edge> edges;
    std::set<std::pair<int,int>> edgeSet; 
    std::vector<Face> faces; //vertex indexes of a face  
    std::vector<Triangle> triangulatedFaces; //vertex indexes of a triangle 
    
    std::map<std::string, Material> materials;

    
    std::vector<Triangle> triangulateFace(const Face& face);
    void triangulate();
    static Mesh parseMeshFromObj(const std::string obj_path);


    void applyQEM(int target_count); 
    void buildConnectivity();
    void updateConnectivityLocal(int v1, int v2);
    void computeVertexQuadrics();
    void computeEdgeCosts();
    void collapseEdge(Edge& edge);

    std::vector<int> getVertexNeighbors(int vertexIndex);
    std::vector<int> getVertexFaces(int vertexIndex);
    std::vector<int> getVertexEdges(int vertexIndex);
    std::set<int> getConnectedVertices(int v);
    
    bool isBoundaryEdge(int v1, int v2);
    Vec3 computeNormal(const Triangle& t);
    bool triangleFlipCheck(int v1, int v2, const Vec3& newPos);
    bool hasMoreThanTwoNeighbors(int v1, int v2);
    bool isBoundaryCheck(int v1, int v2);
  private:
    static void parseMtlFile(const std::string& mtl_path, std::map<std::string, Material>& materials);
    void replaceVertex(int oldV, int newV);
    void removeDegenerateTriangles();
    void computeEdgeCost(Edge& edge);
};


