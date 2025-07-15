#include "Exp.h"

Exp::Exp(float exponent, float fudgeFactor, std::unique_ptr<NoiseFunction> noiseFunction)
{
    this->exponent = exponent;
    this->fudgeFactor = fudgeFactor;
    setNoiseFunction(std::move(noiseFunction));
}

float Exp::getValue(const Vect3 &value) const
{
    return std::pow(this->noiseFunction->getValue(value) * fudgeFactor, this->exponent);
}

oneapi::tbb::flow::sender<float> *Exp::getComputationGraph(oneapi::tbb::flow::graph &graph, oneapi::tbb::flow::sender<Vect3> &input_node)
{
    auto parent = this->noiseFunction->getComputationGraph(graph, input_node);

    auto node = new oneapi::tbb::flow::function_node<float, float>(graph, oneapi::tbb::flow::serial, [this](const float &value)
                                                                   { return std::pow(value * fudgeFactor, this->exponent); });

    oneapi::tbb::flow::make_edge(*parent, *node);

    return node;
}
