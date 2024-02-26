#include <SDL2/SDL.h>

class Renderer {
public:
  Renderer(int cells, int window_size);
  ~Renderer();
  void render();
  void close();
  bool should_close() { return is_quit; };
  const int cells;

private:
  const int window_size;
  bool is_quit = false;
  SDL_Window *window;
  SDL_Surface *window_surface;
  SDL_Renderer *window_renderer;

  void draw_grid();
};