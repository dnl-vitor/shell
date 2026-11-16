cmake_minimum_required(VERSION 3.14)
project(dhell VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMAND ON)

add_executable(${PROJECT_NAME}
  main.c
  shell.c
  builtins.c
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -g)
