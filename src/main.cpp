#include "Renderer.h"
#include "mlx/mlx.h"
#include <chrono>
#include <iostream>

using namespace mlx::core;

int main() {
  Renderer renderer(10, 480);
  while (!renderer.should_close()) {
    auto start = std::chrono::high_resolution_clock::now();
    renderer.render();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Frame time: " << duration.count() << "ms\n";
  }
}