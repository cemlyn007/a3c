#include <SDL2/SDL.h>
#include <vector>
class Viewer {
public:
  Viewer(int cells, int window_size, bool show_grid);
  ~Viewer();
  void render(std::vector<int>& snake, int food);
  void close();
  bool should_close() { return is_quit; };
  int get_action();
  const int cells;

private:
  const int window_size;
  bool is_quit = false;
  bool show_grid = true;
  SDL_Window *window;
  SDL_Surface *window_surface;
  SDL_Renderer *window_renderer;

  void draw_grid();
  void draw_snake(std::vector<int>& snake);
  void draw_food(int food);
};