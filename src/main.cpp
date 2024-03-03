#include "Environment.h"
#include "Viewer.h"
#include "mlx/mlx.h"
#include <chrono>
#include <iostream>
#include <vector>

const int N_CELLS = 10;

std::vector<int> extract_snake(const mlx::core::array& snake, int head_index, int cells) {
  if (!snake.is_evaled()) {
    throw std::runtime_error("The snake array must be evaluated");
  }
  auto head_value = mlx::core::take(snake, {head_index}).item<int>();
  int index = head_index;
  int value = head_value;
  std::vector<int> snake_vector;
  snake_vector.reserve(head_value-1);
  snake_vector.push_back(head_index);
  while (value != 1) {
    int i = 0;
    std::vector<int> adjacent_indices = {index-1, index+1, index-cells, index+cells};
    auto adjacent = mlx::core::take(snake, {index-1, index+1, index-cells, index+cells});
    adjacent.eval();
    for (auto a : adjacent) {
      if (a.item<int>() == value-1) {
        break;
      }
      i++;
    }
    index = adjacent_indices[i];
    value--;
    snake_vector.push_back(index);
  }
  return snake_vector;
}

std::vector<mlx::core::array> step(const std::vector<mlx::core::array> &args) {
  auto state = State{.snake = args[0], .food = args[1]};
  Environment environment(N_CELLS);
  auto new_state = environment.step(state, args[2]);
  return {new_state.snake, new_state.food, new_state.head};
};

int main() {
  int cells = N_CELLS;
  Viewer renderer(cells, 720);

  int food = 25;
  auto mlx_snake = mlx::core::scatter(
    mlx::core::zeros({cells * cells}, mlx::core::int32), 
    mlx::core::array({0, 1, 2}, mlx::core::int32),
    mlx::core::expand_dims(mlx::core::array({1, 2, 3}, mlx::core::int32), 1),
    0
  );
  mlx_snake.eval();
  auto snake = extract_snake(mlx_snake, 2, N_CELLS);
  auto compiled_step = mlx::core::compile(step);

  auto state = State{
      .snake = mlx_snake,
      .food = mlx::core::array(food, mlx::core::int32),
      .head = mlx::core::array(2, mlx::core::int32)
  };
  renderer.render(snake, food);
  while (!renderer.should_close()) {
    auto start_action = std::chrono::high_resolution_clock::now();
    auto action = renderer.get_action();
    if (action == -1) {
      break;
    }
    auto mlx_action = mlx::core::array(action, mlx::core::int32);
    mlx_action.eval();
    auto end_action = std::chrono::high_resolution_clock::now();
    auto duration_action = std::chrono::duration_cast<std::chrono::milliseconds>(end_action - start_action);
    std::cout << "Action duration: " << duration_action.count() << "ms" << std::endl;

    auto start_step = std::chrono::high_resolution_clock::now();
    auto new_state_vector = compiled_step({state.snake, state.food, mlx_action});
    state = State{.snake = new_state_vector[0], .food = new_state_vector[1], .head = new_state_vector[2]};
    state.snake.eval();
    food = state.food.item<int>();
    auto end_step = std::chrono::high_resolution_clock::now();
    auto duration_step = std::chrono::duration_cast<std::chrono::milliseconds>(end_step - start_step);
    std::cout << "Step duration: " << duration_step.count() << "ms" << std::endl;

    std::cout << state.head.size() << std::endl;

    auto start_extract_snake = std::chrono::high_resolution_clock::now();
    snake = extract_snake(state.snake, state.head.item<int>(), N_CELLS);
    auto end_extract_snake = std::chrono::high_resolution_clock::now();
    auto duration_extract_snake = std::chrono::duration_cast<std::chrono::milliseconds>(end_extract_snake - start_extract_snake);
    std::cout << "Extract snake duration: " << duration_extract_snake.count() << "ms" << std::endl;

    auto start_render = std::chrono::high_resolution_clock::now();
    renderer.render(snake, food);
    auto end_render = std::chrono::high_resolution_clock::now();
    auto duration_render = std::chrono::duration_cast<std::chrono::milliseconds>(end_render-start_render);
    std::cout << "Render duration: " << duration_render.count() << "ms" << std::endl;
  }
}