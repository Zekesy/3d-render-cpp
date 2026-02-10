#include <iostream>
#include <SDL2/SDL.h>

int main() {
  std::cout << "Hello World\n";
  
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
      std::cout << "SDL Init Error: " << SDL_GetError() << std::endl;
      return 1;
  }

  SDL_Window* window;
  SDL_Renderer* renderer; 

  int result = SDL_CreateWindowAndRenderer(500, 500, 0, &window, &renderer);
  if( result != 0 ){
    std::cout << "Failed to create window and renderer" << SDL_GetError() << std::endl;
  }

  SDL_SetRenderDrawColor(renderer, 255,255,255,255);
  SDL_RenderDrawPoint(renderer, 100, 100);
  SDL_RenderDrawLine( renderer, 10, 10, 50, 25 ); // Draw a line

  SDL_Rect r;
  r.x = 150;
  r.y = 25;
  r.h = 75;
  r.w = 120;

  SDL_RenderFillRect( renderer, &r ); 

  SDL_RenderPresent (renderer); 
  

  SDL_Delay(3000);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  SDL_Quit();

  return 0; 
}


