#include "Environment.h"
#include "Viewer.h"
#include "mlx/mlx.h"
#include <chrono>
#include <iostream>
#include <vector>

int main() {
  int cells = 10;
  Viewer renderer(cells, 480);
  Environment environment(cells);
  std::vector<int> snake = {0, 1, 2};
  int food = 25;
  renderer.render(snake, food);
  while (!renderer.should_close()) {
    auto start = std::chrono::high_resolution_clock::now();

    auto action = renderer.get_action();
    if (action == -1) {
      break;
    }

    auto state = State{
        .snake = mlx::core::array(snake.data(), {static_cast<int>(snake.size())}, mlx::core::int32),
        .food = mlx::core::array(food, mlx::core::int32)
    };

    auto mlx_action = mlx::core::array(action, mlx::core::int32);
    std::cout << mlx_action.ndim() << std::endl;
    auto new_state = environment.step(state, mlx_action);

    new_state.snake.eval();
    new_state.food.eval();

    if (snake.size() == new_state.snake.size()) {
      for (int i = 0; i < snake.size(); i++) {
        auto index = mlx::core::take(new_state.snake, {i});
        snake[i] = index.item<int>();
      };
    } else {
      snake.clear();
      for (int i = 0; i < new_state.snake.size(); i++) {
        auto index = mlx::core::take(new_state.snake, {i});
        snake.push_back(index.item<int>());
      }
    }

    food = new_state.food.item<int>();

    renderer.render(snake, food);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Frame time: " << duration.count() << "ms\n";
  }
}