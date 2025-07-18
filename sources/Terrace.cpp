#include "Terrace.h"

float Terrace::getValue(const Vect3 &value) const
{
    float noiseValue = noiseFunction->getValue(value);
    // float step = 1.0f / terraces;

    float steppedValue = terraceOperation(noiseValue, terraces);

    return steppedValue;
}

oneapi::tbb::flow::sender<float> *Terrace::getComputationGraph(oneapi::tbb::flow::graph &graph, oneapi::tbb::flow::sender<Vect3> &input_node)
{
    auto parent = this->noiseFunction->getComputationGraph(graph, input_node);

    auto node = new oneapi::tbb::flow::function_node<float, float>(graph, oneapi::tbb::flow::serial, [this](const float &value)
                                                                   { return terraceOperation(value, 1.0f / terraces); });

    oneapi::tbb::flow::make_edge(*parent, *node);

    return node;
}