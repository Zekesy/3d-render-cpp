#include "renderer.h"
#include "mesh.h"
#include "transformation.h"
#include <vector>
#include <algorithm>
#include <cmath>

// Helper functions for lighting
static float calculateLighting(const Vertex& v0, const Vertex& v1, const Vertex& v2) {
  // Calculate two edges of the triangle
  float edge1x = v1.v.data[0] - v0.v.data[0];
  float edge1y = v1.v.data[1] - v0.v.data[1];
  float edge1z = v1.v.data[2] - v0.v.data[2];
  
  float edge2x = v2.v.data[0] - v0.v.data[0];
  float edge2y = v2.v.data[1] - v0.v.data[1];
  float edge2z = v2.v.data[2] - v0.v.data[2];
  
  // Calculate normal via cross product
  float nx = edge1y * edge2z - edge1z * edge2y;
  float ny = edge1z * edge2x - edge1x * edge2z;
  float nz = edge1x * edge2y - edge1y * edge2x;
  
  // Normalize the normal
  float len = std::sqrt(nx*nx + ny*ny + nz*nz);
  if(len < 0.0001f) return 0.5f; // Degenerate triangle
  
  nx /= len;
  ny /= len;
  nz /= len;
  
  // Simple directional light from camera (0, 0, -1)
  float lightX = 0.0f;
  float lightY = 0.0f;
  float lightZ = -1.0f;
  
  // Dot product for diffuse lighting
  float intensity = nx * lightX + ny * lightY + nz * lightZ;
  
  // Clamp and add ambient light
  intensity = std::max(0.0f, intensity);
  float ambient = 0.3f;
  
  return ambient + (1.0f - ambient) * intensity;
}

static uint32_t applyLighting(uint32_t color, float intensity) {
  uint8_t a = (color >> 24) & 0xFF;
  uint8_t r = (color >> 16) & 0xFF;
  uint8_t g = (color >> 8) & 0xFF;
  uint8_t b = color & 0xFF;
  
  r = static_cast<uint8_t>(r * intensity);
  g = static_cast<uint8_t>(g * intensity);
  b = static_cast<uint8_t>(b * intensity);
  
  return (a << 24) | (r << 16) | (g << 8) | b;
}

void Renderer::renderMeshWithTransform(Mesh& mesh, const Transform& transform){
  mesh.triangulate();
  
  std::vector<Vertex> transformedVertices = transformation(mesh.vertices, transform);
  std::vector<ScreenPoint> screenVertices = projectVertices(mesh, transformedVertices, width, height);
  
  clear(); 
  rasterizeTriangles(mesh, screenVertices);
}

std::vector<Vertex> Renderer::transformation(
    const std::vector<Vertex>& vertices,
    const Transform& transform) 
{
  std::vector<Vertex> result;
  result.reserve(vertices.size());

  for(const auto& vertex : vertices){
      auto v = vertex; 

      v.v = rotateX(v.v, transform.rotX);
      v.v = rotateY(v.v, transform.rotY);
      v.v = rotateZ(v.v, transform.rotZ);

      v.v.data[0] += transform.x;
      v.v.data[1] += transform.y;
      v.v.data[2] += transform.z;

      result.push_back(v);  
  }

  return result; 
}

ScreenPoint Renderer::project(const Vertex& vertex, int width, int height, uint32_t color){
  const float fov = 1.0f;  
  const float z = std::max(vertex.v.data[2], 0.1f);
  
  const float x = (vertex.v.data[0] / z) * fov;
  const float y = (vertex.v.data[1] / z) * fov;

  const float screenX = (x + 1.0f) * 0.5f * width;
  const float screenY = (1.0f - y) * 0.5f * height;
  
  return {screenX, screenY, z, color};
}

std::vector<ScreenPoint> Renderer::projectVertices( 
    const Mesh& mesh,
    const std::vector<Vertex>& vertices,  
    int width, int height)
{
  std::vector<ScreenPoint> result;
  result.reserve(vertices.size());

  for(const auto& v : vertices) {
    result.push_back(project(v, width, height, 0xFFFFFFFF));  
  }

  return result; 
}

void Renderer::rasterizeTriangles(const Mesh& mesh, const std::vector<ScreenPoint>& screenVertices){
  for(const auto& tri : mesh.triangulatedFaces) {
    const ScreenPoint& p0 = screenVertices[tri.v1_index];
    const ScreenPoint& p1 = screenVertices[tri.v2_index];
    const ScreenPoint& p2 = screenVertices[tri.v3_index];
    
    if(shouldCullFace(p0, p1, p2)) {
      continue; 
    }
    
    // Get base color
    uint32_t triColor = 0xFFFFFFFF; //default white 
    if(!tri.materialName.empty()) {
      auto it = mesh.materials.find(tri.materialName);
      if(it != mesh.materials.end()){
        triColor = materialToColor(it->second);
      }
    }
    
    // Calculate lighting based on 3D vertex positions
    const Vertex& v0 = mesh.vertices[tri.v1_index];
    const Vertex& v1 = mesh.vertices[tri.v2_index];
    const Vertex& v2 = mesh.vertices[tri.v3_index];
    float lighting = calculateLighting(v0, v1, v2);
    
    // Apply lighting to color
    triColor = applyLighting(triColor, lighting);

    ScreenPoint cp0 = p0; cp0.color = triColor;
    ScreenPoint cp1 = p1; cp1.color = triColor;
    ScreenPoint cp2 = p2; cp2.color = triColor;
    
    if (wireframe) {
      rasterizeTriangleEdges(cp0, cp1, cp2);
    } else {
      rasterizeTriangle(cp0, cp1, cp2);
    }
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
          colorBuffer[bufferIndex] = p0.color;
        }
      }
    }
  }
}

void Renderer::rasterizeTriangleEdges(const ScreenPoint& p0, const ScreenPoint& p1, const ScreenPoint& p2){
  drawLine(p0, p1);
  drawLine(p1, p2);
  drawLine(p2, p0);

}

void Renderer::drawLine(const ScreenPoint& p0, const ScreenPoint& p1){
  float dx = p1.x - p0.x;
  float dy = p1.y - p0.y;
  float dz = p1.depth - p0.depth;
  
  float step = 0.0;
  if(std::abs(dx) > std::abs(dy)){
    step = std::abs(dx);
  } else {
    step = std::abs(dy);
  }
  
  if (step == 0.0f) return;
  //Find slope and stepping direction 
  dx /= step; 
  dy /= step; 
  dz /= step;

  float x = p0.x;
  float y = p0.y;
  float z = p0.depth;

  for(int i = 0; i <= (int)step; i++){
    int ix = (int)(x + 0.5f);
    int iy = (int)(y + 0.5f);
    
   if (ix >= 0 && ix < width && iy >= 0 && iy < height) {  // bounds check
      int bufferIndex = iy * width + ix;
      if (z < depthBuffer[bufferIndex]) {         
        depthBuffer[bufferIndex] = z;
        colorBuffer[bufferIndex] = 0xFF00FF00;        
      }
    }
    
    x += dx;
    y += dy;
    z += dz;  
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
  std::fill(colorBuffer.begin(), colorBuffer.end(), 0xFFFFFFF);
  std::fill(depthBuffer.begin(), depthBuffer.end(), std::numeric_limits<float>::infinity());
}
