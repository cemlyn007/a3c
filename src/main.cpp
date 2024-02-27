#include "Renderer.h"
#include "mlx/mlx.h"
#include <chrono>
#include <iostream>
#include <vector>
using namespace mlx::core;

int main() {
  Renderer renderer(10, 480);
  std::vector<int> snake = {0, 1, 2, 3, 4};
  int food = 25;
  while (!renderer.should_close()) {
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < snake.size(); i++) {
      snake[i] = (snake[i] + 1) % (renderer.cells * renderer.cells);
    }
    
    renderer.render(snake, food);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Frame time: " << duration.count() << "ms\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(60));
  }
}