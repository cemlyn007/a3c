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
  auto mlx_snake = mlx::core::scatter(
    mlx::core::zeros({cells * cells}, mlx::core::int32), 
    mlx::core::array({0, 1, 2}, mlx::core::int32),
    mlx::core::expand_dims(mlx::core::array({1, 2, 3}, mlx::core::int32), 1),
    0
  );
  renderer.render(snake, food);
  while (!renderer.should_close()) {
    auto start = std::chrono::high_resolution_clock::now();

    auto action = renderer.get_action();
    if (action == -1) {
      break;
    }

    auto state = State{
        .snake = mlx_snake,
        .food = mlx::core::array(food, mlx::core::int32)
    };

    std::cout << mlx_snake << std::endl;

    auto mlx_action = mlx::core::array(action, mlx::core::int32);
    auto new_state = environment.step(state, mlx_action);

    new_state.snake.eval();
    auto sorted_indices = mlx::core::argsort(new_state.snake);
    sorted_indices.eval();
    auto sorted_values = mlx::core::take(new_state.snake, sorted_indices);
    sorted_values.eval();

    snake.clear();
    for (int i = 0; i < sorted_indices.size(); i++) {
      auto index = mlx::core::take(sorted_indices, {i}).item<int>();
      auto value = mlx::core::take(sorted_values, {i}).item<int>();
      if (value != 0){
        std::cout << index << " WITH SNAKE BODY PART " << value << std::endl;
        snake.push_back(index);
      }
    };

    food = new_state.food.item<int>();
    mlx_snake = new_state.snake;

    renderer.render(snake, food);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Frame time: " << duration.count() << "ms\n";
  }
}