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

    std::size_t size() const;

    const Neuron & operator[](size_t idx) const;
    Neuron & operator[](size_t idx);

    std::vector<Neuron>::iterator begin();
    std::vector<Neuron>::iterator end();
    std::vector<Neuron>::const_iterator begin() const;
    std::vector<Neuron>::const_iterator end() const;
    std::vector<Neuron>::const_iterator cbegin() const;
    std::vector<Neuron>::const_iterator cend() const;
};


namespace activation {
    typedef std::function<double(double)> Function;
    typedef Function fn;

    double sigmoid(double potential, double lambda);
    fn createSigmoid(double lambda);
};


namespace activ = activation;


class BackpropagationNetwork {
    activ::fn activationFn;

    double lambda = 1.0;
    double eta = 0.1;
    double threshold = 0.1;
    int iterations = 10;

    std::vector<Layer> layers;
    std::mt19937 rnd { std::random_device()() };
    std::uniform_real_distribution<> dist { 0.0, 1.0 };

    double neuronOutput(const Neuron & neuron, const std::vector<double> & inputs);
    double neuronPotential(const Neuron & neuron, const std::vector<double> & inputs);

    void initLayer(size_t layer);

    void validateInput(const std::vector<double> & input);

    std::vector<double> calcLayerValues(size_t layer, const std::vector<double> & inputs);

    std::vector<std::vector<double>> outputsOfAllLayers(const std::vector<double> & inputs);

    double calcNetworkError(const std::vector<double> & signals, size_t expected);

    std::vector<double> adjustOutputLayer(const std::vector<double> & inputs, const std::vector<double> & outputs, size_t expected);

    std::vector<double> adjustHiddenLayer(const std::vector<double> & inputs, const std::vector<double> & outputs, size_t layer, const std::vector<double> & prevErrors, size_t expected);

    bool teachIteration(const std::vector<double> & signals, size_t expected);

public:

    BackpropagationNetwork(activ::fn fun, size_t input, size_t output, size_t hiddenLayers, size_t hiddenLayerNeurons);

    size_t predict(const std::vector<double> & input);
    std::vector<double> outputValues(std::vector<double> input);

    void teach(const std::vector<double> & signals, size_t expected);

};

#endif

