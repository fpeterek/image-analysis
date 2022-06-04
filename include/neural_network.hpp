#ifndef IMAGE_ANALYSIS_NEURAL_NETWORK_HPP
#define IMAGE_ANALYSIS_NEURAL_NETWORK_HPP

#include <cstdint>
#include <cmath>
#include <vector>
#include <functional>
#include <random>


struct Neuron {
    std::vector<double> weights;
};


struct Layer {
    std::vector<Neuron> neurons;
    Layer(size_t size);
};


namespace activation {
    typedef std::function<double(double)> Function;
    typedef Function fn;

    double sigmoid(double potential);
};


namespace activ = activation;


class BackpropagationNetwork {
    activ::fn activationFn;

    std::vector<Layer> layers;
    std::mt19937 rnd { std::random_device()() };
    std::uniform_real_distribution<> dist { 0.0, 1.0 };

    double neuronOutput(const Neuron & neuron, const std::vector<double> & inputs);
    double neuronPotential(const Neuron & neuron, const std::vector<double> & inputs);

    void initLayer(size_t layer);

public:

    BackpropagationNetwork(activ::fn fun, size_t input, size_t output, size_t hiddenLayers, size_t hiddenLayerNeurons);

    size_t predict(const std::vector<double> & input);
    std::vector<double> outputWeight(const std::vector<double> & input);

};

#endif

