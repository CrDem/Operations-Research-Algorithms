cmake_minimum_required(VERSION 3.16)
project(GameLP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  highs
  GIT_REPOSITORY https://github.com/ERGO-Code/HiGHS.git
  GIT_TAG v1.7.2
)

FetchContent_MakeAvailable(highs)

add_executable(game_lp
    src/main.cpp
    src/game_loader.cpp
)

target_compile_definitions(game_lp PRIVATE
  PROJECT_ROOT="${CMAKE_SOURCE_DIR}"
)

target_link_libraries(game_lp highs)