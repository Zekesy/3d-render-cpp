#include <iostream>
#include <SDL2/SDL.h>
#include "renderer.h"
#include "mesh.h"
#include "transformation.h"
#include <vector>
// Create a cube mesh
// Mesh createCube() {
//   Mesh cube;
//
//   // 8 vertices of a cube
//   cube.vertices = {
//     {-1, -1, -1}, // 0
//     { 1, -1, -1}, // 1
//     { 1,  1, -1}, // 2
//     {-1,  1, -1}, // 3
//     {-1, -1,  1}, // 4
//     { 1, -1,  1}, // 5
//     { 1,  1,  1}, // 6
//     {-1,  1,  1}  // 7
//   };
//
//   // 6 faces (each is a quad)
//   cube.faces = {
//     {0, 1, 2, 3}, // Front
//     {1, 5, 6, 2}, // Right
//     {5, 4, 7, 6}, // Back
//     {4, 0, 3, 7}, // Left
//     {3, 2, 6, 7}, // Top
//     {4, 5, 1, 0}  // Bottom
//   };
//
//   return cube;
// }

int main() {
  const int WINDOW_WIDTH = 1500;
  const int WINDOW_HEIGHT = 1000;
  
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cout << "SDL Init Error: " << SDL_GetError() << std::endl;
    return 1;
  }
  
  SDL_Window* window = SDL_CreateWindow(
    "Rotating Shape",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    WINDOW_WIDTH,
    WINDOW_HEIGHT,
    0
  );
  
  if (!window) {
    std::cout << "Window creation error: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return 1;
  }
  
  SDL_Renderer* sdlRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!sdlRenderer) {
    std::cout << "Renderer creation error: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }
  
  // Create texture for our framebuffer
  SDL_Texture* texture = SDL_CreateTexture(
    sdlRenderer,
    SDL_PIXELFORMAT_ARGB8888,
    SDL_TEXTUREACCESS_STREAMING,
    WINDOW_WIDTH,
    WINDOW_HEIGHT
  );
  
  // Create our custom renderer and cube
  Renderer renderer(WINDOW_WIDTH, WINDOW_HEIGHT);
  //Mesh cube = createCube();
  
  Mesh spinningTop = Mesh::parseMeshFromObj("../assets/spinningTop.obj", "../assets/spinningTop.mtl"); 
  // Animation variables
  float rotationX = 0.005f;
  float rotationY = 0.01f;
  float rotationZ = 0.003f;
  bool running = true;
  SDL_Event event;
  
  while (running) {
    // Handle events
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      }
      if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
        running = false;
      }
    }
    
    // Update rotation
    rotationX += 0.0f;
    rotationY += 0.0f;
    rotationZ += 0.01;
    
    // Set up transform
    Transform transform;
    transform.rotX = rotationX;
    transform.rotY = rotationY;
    transform.rotZ = rotationZ;
    transform.x = 0.0f;
    transform.y = 0.0f;
    transform.z = 50.0f;  // Move cube away from camera
    
    // Render the cube
    renderer.renderMeshWithTransform(spinningTop, transform);
    
    // Copy our color buffer to SDL texture
    SDL_UpdateTexture(texture, nullptr, renderer.colorBuffer.data(), WINDOW_WIDTH * sizeof(uint32_t));
    
    // Display
    SDL_RenderClear(sdlRenderer);
    SDL_RenderCopy(sdlRenderer, texture, nullptr, nullptr);
    SDL_RenderPresent(sdlRenderer);
    
    SDL_Delay(16); // ~60 FPS
  }
  
  // Cleanup
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(sdlRenderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  
  return 0;
}
