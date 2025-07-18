#include <oneapi/tbb.h>
#include <iostream>
#include <memory>
#include <vector>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <string>

#include "sources/Vect3.h"
#include "sources/PerlinNoise.h"
#include "sources/ConstantNoise.h"
#include "sources/Add.h"
#include "sources/Exp.h"
#include "sources/Mult.h"
#include "sources/RidgedNoise.h"
#include "sources/Plane.h"
#include "sources/TerrainGenerator.h"

#include "SFML/Graphics/Color.hpp"

constexpr int REPETITIONS = 20;

void writeResults(const std::filesystem::path &resultsPath, const std::vector<long> &results)
{
    std::ofstream file{};
    file.open(resultsPath);
    for (const auto &result : results)
    {
        file << result << std::endl;
    }

    file.close();

    std::cout << "Written: " << results.size() << " tests" << std::endl;
}

int main()
{
    std::vector<int> thread_counts = {1, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24};
    std::vector<std::pair<int, int>> resolutions = {
        {640, 480},   // 2073600 pixels
        {1024, 786},  // 2073600 pixels
        {1920, 1080}, // 2073600 pixels
        {2560, 1440}  // 2073600 pixels
    };

    std::string functions_names[] = {"PerlinNoise", "RidgedNoise", "ComplexNoise"};
    int i = 0;

    std::filesystem::create_directory("benchmark_results");
    std::filesystem::path output_path = "benchmark_results";

    for (const auto &resolution : resolutions)
    {
        std::vector<std::unique_ptr<NoiseFunction>> functions;

        functions.emplace_back(std::make_unique<PerlinNoise>(69, 0.5f, 3));
        functions.emplace_back(std::make_unique<RidgedNoise>(std::make_unique<Exp>(10.0f, 1.2f, std::make_unique<PerlinNoise>(69, 0.5f, 3)), 8));

        functions.emplace_back(std::make_unique<RidgedNoise>(std::make_unique<Add>(
                                                                 std::make_unique<Exp>(10.0f, 1.2f, std::make_unique<PerlinNoise>(6, 0.5f, 3)),
                                                                 std::make_unique<Mult>(
                                                                     std::make_unique<PerlinNoise>(3, 0.5f, 3),
                                                                     std::make_unique<ConstantNoise>(0.25f)),
                                                                 2.0f, 1.0f),
                                                             8));

        int width = resolution.first;
        int height = resolution.second;

        printf("Resolution: %dx%d\n", width, height);

        for (auto &&function : functions)
        {
            printf("Using noise function");
            auto function_name = functions_names[i++];

            std::unique_ptr<Terrain> terrain = std::make_unique<Plane>(std::move(function), 5.0f, 4000.0f);
            auto generator = std::make_unique<TerrainGenerator>(std::move(terrain));

            // SEQUENTIAL TEST
            printf(" - Sequential\n");
            auto result_path = output_path / (std::to_string(width) + "x" + std::to_string(height) + "_" + function_name + "_sequential.txt");
            std::vector<long> sequential_results;

            for (int i = 0; i < REPETITIONS; ++i)
            {
                auto start = std::chrono::high_resolution_clock::now();
                auto colorMap = generator->getColorMap(width, height);
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
                sequential_results.push_back(duration);
            }

            writeResults(result_path, sequential_results);

            for (const auto &threads : thread_counts)
            {
                oneapi::tbb::global_control control(oneapi::tbb::global_control::max_allowed_parallelism, threads);
                printf("Running with %d threads\n", threads);
                // OMP PARALLEL TEST
                printf(" - Parallel\n");
                result_path = output_path / (std::to_string(width) + "x" + std::to_string(height) + "_" + function_name + "_" + std::to_string(threads) + "omp.txt");
                std::vector<long> parallel_omp_results;

                for (int i = 0; i < REPETITIONS; ++i)
                {
                    auto start = std::chrono::high_resolution_clock::now();
                    auto colorMap = generator->getColorMapOMP(width, height);
                    auto end = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
                    parallel_omp_results.push_back(duration);
                }
                writeResults(result_path, parallel_omp_results);

                // Generate height and color maps
                // PARALLEL TEST
                printf(" - Parallel\n");
                result_path = output_path / (std::to_string(width) + "x" + std::to_string(height) + "_" + function_name + "_" + std::to_string(threads) + "parallel.txt");
                std::vector<long> parallel_results;

                for (int i = 0; i < REPETITIONS; ++i)
                {
                    auto start = std::chrono::high_resolution_clock::now();
                    auto colorMap = generator->getColorMapParallel(width, height);
                    auto end = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
                    parallel_results.push_back(duration);
                }
                writeResults(result_path, parallel_results);

                // PARALLEL GRAPH TEST
                printf(" - Parallel GRAPH\n");
                result_path = output_path / (std::to_string(width) + "x" + std::to_string(height) + "_" + function_name + "_" + std::to_string(threads) + "parallel_g.txt");
                std::vector<long> parallel_graph_results;

                for (int i = 0; i < REPETITIONS; ++i)
                {
                    auto start = std::chrono::high_resolution_clock::now();
                    auto colorMap = generator->getColorMapGraph(width, height);
                    auto end = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
                    parallel_graph_results.push_back(duration);
                }

                writeResults(result_path, parallel_graph_results);
            }
        }
    }

    return 0;
}
