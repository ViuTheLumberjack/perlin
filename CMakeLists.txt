cmake_minimum_required(VERSION 3.28)
project(Perlin VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_library(noise
    sources/Add.h
    sources/Add.cpp
    sources/Aggregator.h
    sources/ConstantNoise.h
    sources/Exp.h
    sources/Exp.cpp
    sources/Mult.h
    sources/NoiseFunction.h
    sources/PerlinNoise.h
    sources/PerlinNoise.cpp
    sources/Plane.h
    sources/RidgedNoise.h
    sources/RidgedNoise.cpp
    sources/Sphere.h
    sources/Terrace.h
    sources/Terrace.cpp
    sources/Terrain.h
    sources/Terrain.cpp
    sources/TerrainGenerator.h
    sources/TerrainGenerator.cpp
    sources/Torus.h
    sources/Torus.cpp
    sources/Vect3.h
    sources/Vect3.cpp
    sources/Window.h
    sources/Window.cpp
)

add_executable(renderer
    map_renderer.cpp
)

add_executable(benchmark
    benchmark.cpp
)

find_package(OpenEXR REQUIRED)
find_package(TBB REQUIRED)
find_package(OpenMP REQUIRED)

target_link_libraries(noise m SFML::Graphics SFML::Window SFML::System OpenEXR::OpenEXR TBB::tbb OpenMP::OpenMP_CXX)
target_link_libraries(renderer noise)
target_link_libraries(benchmark noise)
