#include "Environment.h"
#include "mlx/mlx.h"
#include <vector>
#include <iostream>

const int N_CELLS = 10;

std::vector<int> extract_snake(mlx::core::array snake) {
  snake.eval();
  auto sorted_indices = mlx::core::argsort(snake);
  sorted_indices.eval();
  auto sorted_values = mlx::core::take(snake, sorted_indices);
  sorted_values.eval();

  std::vector<int> snake_vector = {};
  for (int i = 0; i < sorted_indices.size(); i++) {
    auto index = mlx::core::take(sorted_indices, {i}).item<int>();
    auto value = mlx::core::take(sorted_values, {i}).item<int>();
    if (value != 0) {
      snake_vector.push_back(index);
    }
  };
  return snake_vector;
}

std::vector<mlx::core::array> step(const std::vector<mlx::core::array> &args) {
  auto state = State{.snake = args[0], .food = args[1]};
  Environment environment(N_CELLS);
  auto new_state = environment.step(state, args[2]);
  return {args[0], args[1]};
};

int main() {
  int cells = N_CELLS;
  Environment environment(cells);
  int food = 25;

  auto mlx_snake = mlx::core::scatter(
    mlx::core::zeros({cells * cells}, mlx::core::int32), 
    mlx::core::array({0, 1, 2}, mlx::core::int32),
    mlx::core::expand_dims(mlx::core::array({1, 2, 3}, mlx::core::int32), 1),
    0
  );

  auto compiled_step = mlx::core::compile(step);

  auto count = 0;

  for (int i = 0; i < cells * cells; i++) {
    auto state = State{.snake = mlx_snake, .food = mlx::core::array(food, mlx::core::int32)};
    auto mlx_action = mlx::core::random::randint(0, 4, {}, mlx::core::int32);
    auto start = std::chrono::high_resolution_clock::now();
    auto new_state_vector = compiled_step({state.snake, state.food, mlx_action});
    state = State{.snake = new_state_vector[0], .food = new_state_vector[1]};
    food = state.food.item<int>();
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds" << std::endl;
    count += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
  }
  std::cout << "Average time: " << count / (cells * cells) << " microseconds" << std::endl;
}
