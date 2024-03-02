#include "Environment.h"
#include <iostream>

int main() {
  int cells = 10;
  Environment environment(cells);
  auto mlx_snake = mlx::core::scatter(
    mlx::core::zeros({cells * cells}, mlx::core::int32), 
    mlx::core::array({0, 1, 2}, mlx::core::int32),
    mlx::core::expand_dims(mlx::core::array({1, 2, 3}, mlx::core::int32), 1),
    0
  );

  int food = 25;
  auto count = 0;
  for (int i = 0; i < cells * cells; i++) {
    auto state = State{.snake = mlx_snake, .food = mlx::core::array(food, mlx::core::int32)};
    auto mlx_action = mlx::core::random::randint(0, 4, {}, mlx::core::int32);
    mlx_action.eval();
    auto start = std::chrono::high_resolution_clock::now();
    auto new_state = environment.step(state, mlx_action);
    food = new_state.food.item<int>();
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "[" << i << "] Time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds" << '\n';
    count += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
  }
  std::cout << "Average time: " << count / (cells * cells) << " microseconds" << std::endl;
}
