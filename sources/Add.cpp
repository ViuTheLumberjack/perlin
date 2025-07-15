#include "Add.h"

float Add::getValue(const Vect3 &value) const
{
    return (this->noiseFunction->getValue(value) * scale1 + otherNoiseFunction->getValue(value) * scale2) / (fabs(scale1) + fabs(scale2));
}

oneapi::tbb::flow::sender<float> *Add::getComputationGraph(oneapi::tbb::flow::graph &graph, oneapi::tbb::flow::sender<Vect3> &input_node)
{
    auto parent1 = this->noiseFunction->getComputationGraph(graph, input_node);
    auto parent2 = this->otherNoiseFunction->getComputationGraph(graph, input_node);

    auto join = new oneapi::tbb::flow::join_node<std::tuple<float, float>>(graph);

    oneapi::tbb::flow::make_edge(*parent1, get<0>(join->input_ports()));
    oneapi::tbb::flow::make_edge(*parent2, get<1>(join->input_ports()));

    auto node = new oneapi::tbb::flow::function_node<std::tuple<float, float>, float>(
        graph, oneapi::tbb::flow::unlimited,
        [this](const std::tuple<float, float> &input)
        {
            const auto [first, second] = input;
            return (first * scale1 + second * scale2) / (fabs(scale1) + fabs(scale2));
        });

    oneapi::tbb::flow::make_edge(*join, *node);

    return node;
}
