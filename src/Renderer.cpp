#include "Renderer.h"
#include <stdexcept>

Renderer::Renderer(int cells, int window_size)
    : cells(cells), window_size(window_size) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    throw std::runtime_error("Failed to initialize the SDL2 library");
  }

  window =
      SDL_CreateWindow("Snake", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, window_size, window_size, 0);

  if (!window) {
    throw std::runtime_error("Failed to create window");
  }

  window_surface = SDL_GetWindowSurface(window);

  window_renderer = SDL_GetRenderer(window);

  if (!window_surface) {
    throw std::runtime_error("Failed to get the surface from the window");
  }
}

void Renderer::render() {
  SDL_Event event;
  if (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      is_quit = true;
    }
  }

  SDL_RenderClear(window_renderer);

  draw_grid();

  SDL_SetRenderDrawColor(window_renderer, 0, 0, 0, 255);

  SDL_RenderPresent(window_renderer);
}

void Renderer::draw_grid() {
  SDL_SetRenderDrawColor(window_renderer, 255, 255, 255, 255);
  for (int i = 0; i < cells; i++) {
    int x = i * (window_size / cells);
    int y = i * (window_size / cells);
    // Draw the horizontal lines
    { SDL_RenderDrawLine(window_renderer, 0, y, window_size, y); }
    // Draw the vertical lines
    { SDL_RenderDrawLine(window_renderer, x, 0, x, window_size); }
  }
}

Renderer::~Renderer() { close(); }

void Renderer::close() {
  SDL_DestroyWindow(window);
  SDL_Quit();
}
