#pragma once 


struct Edge {
  int v1;
  int v2; 
  float error; 
  Vec3 collapsePosition;

 Edge(int a, int b) : v1(a), v2(b), error(0.0f) {}

  bool operator<(const Edge& other) const {
      if (v1 != other.v1) return v1 < other.v1;
      return v2 < other.v2;
  }
  static void computeEdgeCost(Edge& edge);
};
