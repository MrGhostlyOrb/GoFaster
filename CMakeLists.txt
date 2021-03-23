cmake_minimum_required(VERSION 3.17)
project(Racer)

find_package(Freetype REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(OpenGlLinkers -lglut -lGLU -lGL -lGLEW -lfreeimage -lfreetype)

add_executable(Racer main.cpp shaders/Shader.cpp shaders/Shader.h Square.h Square.cpp Circle.h Circle.cpp ImageLoading.cpp ImageLoading.h Car.cpp Car.h OBB.cpp OBB.h FreeImage.h FreeType.h FreeType.cpp)
target_link_libraries(Racer ${OpenGlLinkers} ${FREETYPE_LIBRARIES})
target_include_directories(Racer PRIVATE ${FREETYPE_INCLUDE_DIRS})