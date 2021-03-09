cmake_minimum_required(VERSION 3.17)
project(Racer)

set(CMAKE_CXX_STANDARD 20)
set(OpenGlLinkers -lglut -lGLU -lGL -lGLEW)

add_executable(Racer main.cpp shaders/Shader.cpp shaders/Shader.h Square.h Square.cpp Circle.h Circle.cpp cmake-build-debug/shaders/Shader.cpp cmake-build-debug/shaders/Shader.h)
target_link_libraries(Racer ${OpenGlLinkers})