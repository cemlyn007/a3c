#include "Viewer.h"
#include <stdexcept>

Viewer::Viewer(int cells, int window_size)
    : cells(cells), window_size(window_size) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    throw std::runtime_error("Failed to initialize the SDL2 library");
  }

  window =
      SDL_CreateWindow("Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       window_size, window_size, 0);

  if (!window) {
    throw std::runtime_error("Failed to create window");
  }

  window_surface = SDL_GetWindowSurface(window);

  window_renderer = SDL_GetRenderer(window);

  if (!window_surface) {
    throw std::runtime_error("Failed to get the surface from the window");
  }
}

int Viewer::get_action() {
  SDL_Event event;
  while (!is_quit) {
    if (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        is_quit = true;
        return -1;
      } else if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
        case SDLK_LEFT:
          return 0;
        case SDLK_RIGHT:
          return 1;
        case SDLK_UP:
          return 2;
        case SDLK_DOWN:
          return 3;
        }
      }
    }
  }
  if (SDL_PollEvent(&event)) {
    if (event.type == SDL_KEYUP) {
      is_quit = true;
    }
  }
  return -1;
}

void Viewer::render(std::vector<int> &snake, int food) {
  SDL_Event event;
  if (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      is_quit = true;
    }
  }

  SDL_RenderClear(window_renderer);

  draw_grid();
  draw_snake(snake);
  draw_food(food);

  SDL_SetRenderDrawColor(window_renderer, 0, 0, 0, 255);

  SDL_RenderFlush(window_renderer);

  SDL_RenderPresent(window_renderer);
}

void Viewer::draw_grid() {
  SDL_SetRenderDrawColor(window_renderer, 255, 255, 255, 255);
  for (int i = 0; i < cells; i++) {
    int v = (i * window_size) / cells;
    // Draw the horizontal lines
    SDL_RenderDrawLine(window_renderer, 0, v, window_size, v); 
    // Draw the vertical lines
    SDL_RenderDrawLine(window_renderer, v, 0, v, window_size);
  }
}

void Viewer::draw_snake(std::vector<int> &snake) {
  SDL_SetRenderDrawColor(window_renderer, 0, 255, 0, 255);
  for (int i = 0; i < snake.size(); i++) {
    int column = (snake[i] % cells);
    int row = (snake[i] / cells);
    int x = (column * window_size) / cells;
    int y = (row * window_size) / cells;
    int x1 = ((column + 1) * window_size) / cells;
    int y1 = ((row + 1) * window_size) / cells;
    int width = x1 - x;
    int height = y1 - y;
    SDL_Rect rect = {x, y, width, height};
    SDL_RenderFillRect(window_renderer, &rect);
  }
}

void Viewer::draw_food(int food) {
  SDL_SetRenderDrawColor(window_renderer, 255, 0, 0, 255);
  int column = (food % cells);
  int row = (food / cells);
  int x = (column * window_size) / cells;
  int y = (row * window_size) / cells;
  int x1 = ((column + 1) * window_size) / cells;
  int y1 = ((row + 1) * window_size) / cells;
  int width = x1 - x;
  int height = y1 - y;
  SDL_Rect rect = {x, y, width, height};
  SDL_RenderFillRect(window_renderer, &rect);
}

Viewer::~Viewer() { close(); }

void Viewer::close() {
  SDL_DestroyWindow(window);
  SDL_Quit();
}
