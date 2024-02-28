#include <tuple>
#include "mlx/mlx.h"


struct State {
  mlx::core::array snake;
  mlx::core::array food;
};

class Environment {
public:
  Environment(int cells);
  State step(State& state, mlx::core::array& action);
private:
  const int cells;
};
