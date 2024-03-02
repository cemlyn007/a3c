#include "mlx/mlx.h"


struct State {
  mlx::core::array snake;
  mlx::core::array food;
};

class Environment {
public:
  Environment(int cells);
  State step(const State& state, const mlx::core::array& action);
private:
  const int cells;
};
