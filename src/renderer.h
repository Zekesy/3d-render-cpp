#pragma once 
#include "mesh.h"
#include "transformation.h"
#include <vector>
#include <limits>
#include <cstdint>

struct ScreenPoint {
  float x; 
  float y; 
  float depth; //for z-buffer
};

class Renderer {
  public:
    int width; 
    int height; 

    std::vector<uint32_t> colorBuffer;
    std::vector<float> depthBuffer; 
    
    Renderer (int w, int h)
      : width(w), height(h),
      colorBuffer(w * h),
      depthBuffer(w * h) {}

    void renderMeshWithTransform(Mesh& mesh, const Transform&  transform);
    void clear();
    std::vector<Vertex> transformation(const std::vector<Vertex>& vertices, const Transform& transform);
    ScreenPoint project(const Vertex& vertex, int width, int height);
    std::vector<ScreenPoint> projectVertices(const std::vector<Vertex>& vertices, int width, int height);
    void rasterizeTriangles(const Mesh& mesh, const std::vector<ScreenPoint>& screenVertices);
    void rasterizeTriangle(const ScreenPoint& p0, const ScreenPoint& p1, const ScreenPoint& p2);
    bool barycentricCoordinates(int px, int py, const ScreenPoint& p0, const ScreenPoint& p1, const ScreenPoint& p2, float& w0, float& w1, float& w2);
    bool shouldCullFace(const ScreenPoint& p0, const ScreenPoint& p1, const ScreenPoint& p2);
};
