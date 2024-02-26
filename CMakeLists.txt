cmake_minimum_required(VERSION 3.27)
project(a3c LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
# Note that this needs to change on different systems.
set(PKG_CONFIG_EXECUTABLE /Users/cemlyn/conda/envs/cpp/bin/pkg-config)

set(MLX_BUILD_METAL ON)
include(FetchContent)
FetchContent_Declare(
    mlx
    GIT_REPOSITORY "https://github.com/ml-explore/mlx.git"
    GIT_TAG        "ac02cf33bd77454ce3c3bcb5eec571b5cd6766d6"
)
FetchContent_MakeAvailable(mlx)

FetchContent_Declare(
  sdl
  URL      https://github.com/libsdl-org/SDL/archive/refs/tags/release-2.30.0.tar.gz
  URL_HASH MD5=a7e0175109c59a8e9797b0a2379906e2
)
FetchContent_MakeAvailable(sdl)

add_executable(a3c src/main.cpp src/Renderer.cpp)
target_include_directories(a3c PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(a3c PRIVATE mlx SDL2::SDL2)