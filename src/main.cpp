#include <iostream>
#include <SDL2/SDL.h>
#include "renderer.h"
#include "mesh.h"
#include "transformation.h"
#include <vector>


//ImGui 
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <stdio.h>

#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif 


int main() {
  const int WINDOW_HEIGHT = 800;
  const int WINDOW_WIDTH = 1280;
  float main_scale = ImGui_ImplSDL2_GetContentScaleForDisplay(0);
  SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
  SDL_Window* window = SDL_CreateWindow(
      "Dear ImGui SDL2+SDL_Renderer example", 
      SDL_WINDOWPOS_CENTERED, 
      SDL_WINDOWPOS_CENTERED, 
      (int)(WINDOW_WIDTH * main_scale), 
      (int)(WINDOW_HEIGHT * main_scale), window_flags
  );
  if (window == nullptr)
  {
      printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
      return 1;
  }
  SDL_Renderer* sdlRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
  if (sdlRenderer == nullptr)
  {
      SDL_Log("Error creating SDL_Renderer!");
      return 1;
  }
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  //ImGui::StyleColorsLight();

  // Setup scaling
  ImGuiStyle& style = ImGui::GetStyle();
  style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
  style.FontScaleDpi = main_scale;        // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)
  

  ImGui_ImplSDL2_InitForSDLRenderer(window, sdlRenderer);
  ImGui_ImplSDLRenderer2_Init(sdlRenderer);

  // Our state 
  bool show_demo_window = true;
  bool show_another_window = false;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  // const int WINDOW_WIDTH = 1500;
  // const int WINDOW_HEIGHT = 1000;
  //
  // if (SDL_Init(SDL_INIT_VIDEO) != 0) {
  //   std::cout << "SDL Init Error: " << SDL_GetError() << std::endl;
  //   return 1;
  // }
  //
  // SDL_Window* window = SDL_CreateWindow(
  //   "Rotating Shape",
  //   SDL_WINDOWPOS_CENTERED,
  //   SDL_WINDOWPOS_CENTERED,
  //   WINDOW_WIDTH,
  //   WINDOW_HEIGHT,
  //   0
  // );
  //
  // if (!window) {
  //   std::cout << "Window creation error: " << SDL_GetError() << std::endl;
  //   SDL_Quit();
  //   return 1;
  // }
  //
  // SDL_Renderer* sdlRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  // if (!sdlRenderer) {
  //   std::cout << "Renderer creation error: " << SDL_GetError() << std::endl;
  //   SDL_DestroyWindow(window);
  //   SDL_Quit();
  //   return 1;
  // }
  
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
    
  float rotXStep, rotYStep, rotZStep = 0.0f; 
  float transformZ = 50.0f; 
  bool running = true;
  while (running) {
    // Handle events  
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      if (event.type == SDL_QUIT) {
        running = false;
      }
      if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window)) {
        running = false;
      }
    }
    if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
    {
        SDL_Delay(10);
        continue;
    }
    
    // Update rotation
    rotationX += rotXStep;
    rotationY += rotYStep;
    rotationZ += rotZStep;
    
    // Set up transform
    Transform transform;
    transform.rotX = rotationX;
    transform.rotY = rotationY;
    transform.rotZ = rotationZ;
    transform.x = 0.0f;
    transform.y = 0.0f;
    transform.z = transformZ;  // Move cube away from camera
    
    // Render the cube
    renderer.renderMeshWithTransform(spinningTop, transform);
    
    // Copy our color buffer to SDL texture
    SDL_UpdateTexture(texture, nullptr, renderer.colorBuffer.data(), WINDOW_WIDTH * sizeof(uint32_t));
      
    //Display 
    SDL_RenderSetScale(sdlRenderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    SDL_RenderClear(sdlRenderer);
    SDL_RenderCopy(sdlRenderer, texture, nullptr, nullptr);
    
    // Start the Dear ImGui frame
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("Settings");                          // Create a window called "Hello, world!" and append into it.

    ImGui::Text("Settings");               // Display some text (you can use a format strings too)
    ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
    ImGui::Checkbox("Another Window", &show_another_window);

    ImGui::SliderFloat("Zoom", &transformZ, 20.0f, 100.0f);  //transform z 
    ImGui::SliderFloat("Rotation X", &rotXStep, 0.0f, 1.0f); //rotation x 
    ImGui::SliderFloat("Rotation Y", &rotYStep, 0.0f, 1.0f); //rotation y 
    ImGui::SliderFloat("Rotation Z", &rotZStep, 0.0f, 1.0f); //rotation z 
                           
    ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

    if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
        counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();

    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), sdlRenderer);

    SDL_RenderPresent(sdlRenderer);
  }
  
  // Cleanup
  ImGui_ImplSDLRenderer2_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(sdlRenderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  
  return 0;
}
