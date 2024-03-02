#include "Environment.h"
#include "mlx/random.h"

const int EMPTY = 0;

Environment::Environment(int cells) : cells(cells) {}

// false values in the mask are masked out.
mlx::core::array masked_categorical(mlx::core::array &array, mlx::core::array &mask, int axis) {
  if (array.ndim() != 1 || mask.ndim() != 1) {
    throw std::invalid_argument("array and mask must be 1D arrays");
  } else if (mask.dtype() != mlx::core::bool_) {
    throw std::invalid_argument("mask must be bool");
  } else if (array.size() != mask.size()) {
    throw std::invalid_argument("array and mask must have the same size");
  } else if (!mask.is_tracer() && mlx::core::any(mask).item<bool>() == false) {
    throw std::invalid_argument("mask must have at least one true value");
  }
  return mlx::core::random::categorical(mlx::core::where(mask, array, {-std::numeric_limits<double>::infinity()}), axis);
}

// action 0, 1, 2, 3 correspond to left, right, up, down.
State Environment::step(const State &state, const mlx::core::array &action) {
  if (state.snake.dtype() == mlx::core::uint8 || state.snake.dtype() == mlx::core::uint16 || state.snake.dtype() == mlx::core::uint32 || state.snake.dtype() == mlx::core::uint64) {
    throw std::invalid_argument("snake must be an array of int32");
  } else if (state.snake.ndim() != 1) {
    throw std::invalid_argument("snake must be a 1D array");
  } else if (state.snake.size() != cells * cells) {
    throw std::invalid_argument("snake must have the same size as the grid");
  } else if (state.food.size() != 1) {
    throw std::invalid_argument("food must be a scalar");
  } else if (state.food.ndim() != 0) {
    throw std::invalid_argument("food must be a scalar");
  } else if (action.size() != 1) {
    throw std::invalid_argument("action must be a scalar");
  } else if (action.ndim() != 0) {
    throw std::invalid_argument("action must be a scalar");
  } else if (action.dtype() != mlx::core::int32) {
    throw std::invalid_argument("action must be an int32");
  } else if (!action.is_tracer() && (mlx::core::any(action < 0).item<bool>() || mlx::core::any(action > 3).item<bool>())) {
    throw std::invalid_argument("action must be 0, 1, 2, or 3");  
  }

  mlx::core::array action_map = mlx::core::array({-1, 1, -cells, cells}, mlx::core::int32);
  auto select_action = mlx::core::take(action_map, action);

  auto head = mlx::core::argmax(state.snake);

  auto head_value = mlx::core::copy(mlx::core::take(state.snake, head));
  auto tail = mlx::core::argmin(mlx::core::where(state.snake == EMPTY, {std::numeric_limits<int>::max()}, state.snake));

  auto head_location = mlx::core::divmod(head, {cells});
  if (head_location.size() != 2) {
    throw std::runtime_error("head_location must be a 1D array of int32");
  }
  auto head_x = mlx::core::squeeze(head_location[0], 0);
  auto head_y = mlx::core::squeeze(head_location[1], 0);
  if (head_x.size() != 1 || head_y.size() != 1) {
    throw std::runtime_error("head_x and head_y must be scalar");
  }

  auto next_head = head + select_action;
  auto new_head_location = mlx::core::divmod(next_head, {cells});
  if (new_head_location.size() != 2) {
    throw std::runtime_error("new_head_location must be a 1D array of int32");
  }
  auto new_head_x = mlx::core::squeeze(new_head_location[0], 0);
  auto new_head_y = mlx::core::squeeze(new_head_location[1], 0);
  if (new_head_x.size() != 1 || new_head_y.size() != 1) {
    throw std::runtime_error("head_x and head_y must be scalar");
  }

  // Check that the new head is not outside of the grid:
  auto next_head_mask = mlx::core::logical_and(mlx::core::abs(new_head_x - head_x) <= 1, mlx::core::abs(new_head_y - head_y) <= 1);
  next_head_mask = mlx::core::logical_and(next_head_mask, next_head >= 0);
  next_head_mask = mlx::core::logical_and(next_head_mask, next_head < cells*cells);

  if (next_head_mask.size() != 1) {
    throw std::runtime_error("1 next_head_mask must be a scalar");
  } 
  if (next_head_mask.ndim() != 0) {
    throw std::runtime_error("1 next_head_mask must be a scalar");
  }

  // Check that the new head is not in the snake:
  next_head_mask = mlx::core::logical_and(next_head_mask, mlx::core::reshape(mlx::core::gather(state.snake, next_head, 0, {1}), {}) == EMPTY);

  next_head = mlx::core::where(next_head_mask, next_head, head);

  if (next_head.size() != 1) {
    throw std::runtime_error("next_head must be a scalar");
  }
  if (next_head.ndim() != 0) {
    throw std::runtime_error("next_head must be a scalar");
  }

  auto next_snake = mlx::core::array(state.snake);
  next_snake = next_snake - 1;
  // Add the head to the snake body if the head moved:
  next_snake = mlx::core::where(next_head_mask, mlx::core::scatter(mlx::core::copy(next_snake), next_head, mlx::core::expand_dims(head_value, 0), 0), next_snake);
  next_snake = mlx::core::where(next_snake < EMPTY, next_snake + 1, next_snake);
  next_snake = mlx::core::where(next_head_mask, next_snake, state.snake);

  // Calculate where the new food would go should the old one be eaten:
  auto next_snake_mask = next_snake != EMPTY;
  auto valid_food_mask = next_snake_mask == EMPTY;
  auto logits = mlx::core::ones({cells * cells});
  auto next_food = masked_categorical(logits, valid_food_mask, 0);

  auto snake_ate_food = mlx::core::all(next_head == state.food);
  next_food = mlx::core::where(snake_ate_food, next_food, state.food);
  
  // Add the tail back if we ate food:
  next_snake = mlx::core::where(
    snake_ate_food,  
    mlx::core::scatter(
      mlx::core::copy(next_snake), 
      tail, 
      {1},
       0
    ) + mlx::core::where(next_snake_mask, {1}, {0}),
    next_snake);

  return State{next_snake, next_food};
}
