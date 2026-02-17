#include "renderer.h"
#include "mesh.h"
#include "transformation.h"
#include <vector>
#include <algorithm>
#include <cmath>

void Renderer::renderMeshWithTransform(Mesh& mesh, const Transform& transform){  // REMOVED width, height
  mesh.triangulate();
  
  std::vector<Vertex> transformedVertices = transformation(mesh.vertices, transform);
  std::vector<ScreenPoint> screenVertices = projectVertices(transformedVertices, width, height);
  
  clear(); 
  rasterizeTriangles(mesh, screenVertices);
}

std::vector<Vertex> Renderer::transformation(
    const std::vector<Vertex>& vertices,  // ADDED const
    const Transform& transform) 
{
  std::vector<Vertex> result;
  result.reserve(vertices.size());

  for(auto v : vertices){
      v = rotateX(v, transform.rotX);
      v = rotateY(v, transform.rotY);
      v = rotateZ(v, transform.rotZ);

      v.x += transform.x;
      v.y += transform.y;
      v.z += transform.z;

      result.push_back(v);  
  }

  return result; 
}

ScreenPoint Renderer::project(const Vertex& vertex, int width, int height){
  const float fov = 2.5f;
  const float z = std::max(vertex.z, 0.1f);  // CHANGED 0.1 to 0.1f
  
  const float x = (vertex.x / z) * fov;
  const float y = (vertex.y / z) * fov;

  const float screenX = (x + 1.0f) * 0.5f * width;
  const float screenY = (1.0f - y) * 0.5f * height;
  
  return {screenX, screenY, z};
}

std::vector<ScreenPoint> Renderer::projectVertices(
    const std::vector<Vertex>& vertices,  // ADDED const&
    int width, int height)
{
  std::vector<ScreenPoint> result;
  result.reserve(vertices.size());

  for(const auto& v : vertices) {
    result.push_back(project(v, width, height));  
  }

  return result; 
}

void Renderer::rasterizeTriangles(const Mesh& mesh, const std::vector<ScreenPoint>& screenVertices){  // ADDED Renderer::
  for(const auto& tri : mesh.triangulatedFaces) {
    const ScreenPoint& p0 = screenVertices[tri.v1_index];
    const ScreenPoint& p1 = screenVertices[tri.v2_index];
    const ScreenPoint& p2 = screenVertices[tri.v3_index];
    
    if(shouldCullFace(p0, p1, p2)) {
      continue; 
    }

    rasterizeTriangle(p0, p1, p2);
  }
}

void Renderer::rasterizeTriangle(const ScreenPoint& p0, const ScreenPoint& p1, const ScreenPoint& p2){  
  int minX = std::max(0, (int)std::min({p0.x, p1.x, p2.x}));
  int maxX = std::min(width - 1, (int)std::max({p0.x, p1.x, p2.x}));  
  int minY = std::max(0, (int)std::min({p0.y, p1.y, p2.y}));
  int maxY = std::min(height - 1, (int)std::max({p0.y, p1.y, p2.y}));  

  for(int y = minY; y <= maxY; y++){
    for(int x = minX; x <= maxX; x++){  
      float w0, w1, w2; 
      if(barycentricCoordinates(x, y, p0, p1, p2, w0, w1, w2)){
        float depth = w0 * p0.depth + w1 * p1.depth + w2 * p2.depth;

        int bufferIndex = y * width + x;
        if(depth < depthBuffer[bufferIndex]){
          depthBuffer[bufferIndex] = depth;
          colorBuffer[bufferIndex] = 0xFFFFFFFF;
        }
      }
    }
  }
}

bool Renderer::barycentricCoordinates(
    int px, int py, 
    const ScreenPoint& p0, const ScreenPoint& p1, const ScreenPoint& p2,  
    float& w0, float& w1, float& w2) 
{
  float x = px + 0.5f; 
  float y = py + 0.5f;
  
  auto edgeFunction = [](float ax, float ay, float bx, float by, float cx, float cy) {
    return (cx - ax) * (by - ay) - (cy - ay) * (bx - ax);
  };

  float area = edgeFunction(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y);
    
  if(std::abs(area) < 0.001f) return false;
  
  w0 = edgeFunction(p1.x, p1.y, p2.x, p2.y, x, y) / area;
  w1 = edgeFunction(p2.x, p2.y, p0.x, p0.y, x, y) / area;
  w2 = edgeFunction(p0.x, p0.y, p1.x, p1.y, x, y) / area;
  
  return (w0 >= 0) && (w1 >= 0) && (w2 >= 0);
}

bool Renderer::shouldCullFace(const ScreenPoint& p0, const ScreenPoint& p1, const ScreenPoint& p2){  
  const float edge1x = p1.x - p0.x;
  const float edge1y = p1.y - p0.y;
  const float edge2x = p2.x - p0.x;
  const float edge2y = p2.y - p0.y;  
  
  const float cross = edge1x * edge2y - edge1y * edge2x;

  return (cross <= 0); 
}

void Renderer::clear() {  
  std::fill(colorBuffer.begin(), colorBuffer.end(), 0xFF000000);
  std::fill(depthBuffer.begin(), depthBuffer.end(), std::numeric_limits<float>::infinity());
}
