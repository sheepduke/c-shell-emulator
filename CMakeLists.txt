cmake_minimum_required(VERSION 3.9.0)

project(shell C)

include_directories(
  "src"
  "src/util"
  "src/shell")

file(GLOB_RECURSE SOURCE
  "src/*.c")

add_executable(shell ${SOURCE})

set_property(TARGET shell PROPERTY C_STANDARD 99)

# Treat warning as error.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")
