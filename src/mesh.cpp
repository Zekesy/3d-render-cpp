//Functions for mesh processing
#include <vector>

class Triangle {
  public:
    int v1_index;
    int v2_index;
    int v3_index; 
};

class Mesh {
   //vertices 
  public:
    std::vector<float> vertices; 
    std::vector<std::vector<int>> faces; 
    std::vector<Triangle> triangulatedFaces; 

    std::vector<Triangle> triangulateFace(const std::vector<int>& face) {
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

    std::vector<Triangle> triangulate() {
      std::vector<Triangle> triangles;
      int n = faces.size();
      for(const auto& face : faces){
        std::vector<Triangle> faceTriangles = triangulateFace(face);
        
        triangles.insert( // Insert all elements in [first,last) before pos 
          triangles.end(), //position
          faceTriangles.begin(), //first element
          faceTriangles.end() //one past last 
        );
      }

      return triangles; 
    }
};

