#include "Environment.h"
#include "mlx/random.h"
#include <iostream>

Environment::Environment(int cells) : cells(cells) {}

// false values in the mask are masked out.
mlx::core::array masked_categorical(mlx::core::array &array, mlx::core::array &mask, int axis) {
  if (array.ndim() != 1 || mask.ndim() != 1) {
    throw std::invalid_argument("array and mask must be 1D arrays");
  } else if (mask.dtype() != mlx::core::bool_) {
    throw std::invalid_argument("mask must be bool");
  } else if (array.size() != mask.size()) {
    throw std::invalid_argument("array and mask must have the same size");
  } else if (mlx::core::any(mask).item<bool>() == false) {
    throw std::invalid_argument("mask must have at least one true value");
  }
  return mlx::core::random::categorical(mlx::core::where(mask, array, {-std::numeric_limits<double>::infinity()}), axis);
}

// action 0, 1, 2, 3 correspond to left, right, up, down.
State Environment::step(State &state, mlx::core::array &action) {
  if (state.snake.ndim() != 1 || state.snake.dtype() != mlx::core::int32) {
    throw std::invalid_argument("state.snake must be a 1D array of int32");
  } else if ((state.food.ndim() != 0) || (state.food.dtype() != mlx::core::int32)) {
    std::cout << state.food.ndim() << " - " << state.food.dtype() << std::endl;
    throw std::invalid_argument("state.food must be a scalar int32");
  } else if (action.ndim() != 0 || action.dtype() != mlx::core::int32) {
    throw std::invalid_argument("action must be a 1D array of int32");
  }
  // Move the rest of the snake, now I just need to add the new head
  mlx::core::array action_map = mlx::core::array({-1, 1, -cells, cells}, mlx::core::int32);
  auto select_action = mlx::core::take(action_map, action);

  auto head = mlx::core::take(state.snake, {static_cast<int>(state.snake.size()) - 1});
  auto new_head = (head + select_action) % (cells * cells);

  if (mlx::core::any(new_head == state.snake).item<bool>()) {
    return State{.snake = state.snake, .food = state.food};
  } else if ((new_head == state.food).item<bool>()) {
    auto snake = mlx::core::concatenate({state.snake, new_head});
    auto food_mask = mlx::core::ones({cells * cells}, mlx::core::bool_);
    auto false_values = mlx::core::zeros({static_cast<int>(snake.size()), 1}, mlx::core::bool_);
    food_mask = mlx::core::scatter(food_mask, snake, false_values, 0);
    auto logits = mlx::core::ones({cells * cells});
    auto food = masked_categorical(logits, food_mask, 0);
    return State{.snake = mlx::core::concatenate({state.snake, new_head}), .food = food};
  } else {
    auto snake = mlx::core::slice(state.snake, {1},
                                  {static_cast<int>(state.snake.size())});
    snake = mlx::core::concatenate({snake, new_head});
    return State{.snake = snake, .food = state.food};
  }
}
