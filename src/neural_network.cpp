#include "neural_network.hpp"
#include <SFML/Graphics/PrimitiveType.hpp>
#include <stdexcept>


Layer::Layer(size_t size) : neurons(size) { }

const Neuron & Layer::operator[](const size_t idx) const {
    return neurons[idx];
}

Neuron & Layer::operator[](const size_t idx) {
    return neurons[idx];
}

std::size_t Layer::size() const {
    return neurons.size();
}

std::vector<Neuron>::iterator Layer::begin() {
    return neurons.begin();
}

std::vector<Neuron>::iterator Layer::end() {
    return neurons.end();
}

std::vector<Neuron>::const_iterator Layer::begin() const {
    return neurons.begin();
}

std::vector<Neuron>::const_iterator Layer::end() const {
    return neurons.end();
}

std::vector<Neuron>::const_iterator Layer::cbegin() const {
    return neurons.cbegin();
}

std::vector<Neuron>::const_iterator Layer::cend() const {
    return neurons.cend();
}

namespace activation {

    double sigmoid(const double potential, const double lambda) {
        const auto exp = std::exp(-1 * lambda * potential);
        return 1.0 / (1 + exp);
    }

    fn createSigmoid(const double lambda) {
        return [lambda](const double potential) -> double {
            return sigmoid(potential, lambda);
        };
    }
    
}


BackpropagationNetwork::BackpropagationNetwork(
        activ::fn fun, 
        const size_t input, 
        const size_t output, 
        const size_t hiddenLayers, 
        const size_t hiddenLayerNeurons
    ) : activationFn(std::move(fun)) {

    layers.reserve(hiddenLayers + 2);

    layers.emplace_back(input);

    for (std::size_t i = 0; i < hiddenLayers; ++i) {
        layers.emplace_back(hiddenLayerNeurons);
        initLayer(i);
    }

    layers.emplace_back(output);

}

void BackpropagationNetwork::initLayer(const size_t layerIdx) {
    if (not layerIdx) {
        return;
    }

    const auto weigths = layers[layerIdx-1].neurons.size();
    auto layer = layers[layerIdx];

    for (auto & neuron : layer) {
        neuron.weights.reserve(weigths);
        for (size_t i = 0; i < weigths; ++i) {
            neuron.weights.emplace_back(dist(rnd));
        }
    }
}


size_t BackpropagationNetwork::predict(const std::vector<double> & input) {

    const auto out = outputValues(input);

    double maxVal = 0;
    size_t maxIdx = 0;

    for (size_t i = 0; i != out.size(); ++i) {
        if (out[i] > maxVal) {
            maxVal = out[i];
            maxIdx = i;
        }
    }

    return maxIdx;
}

void BackpropagationNetwork::validateInput(const std::vector<double> & input) {
    if (input.size() != layers.front().size()) {
        throw std::runtime_error("Input size does not match number of neurons in network input layer.");
    }
}

double BackpropagationNetwork::neuronOutput(const Neuron & neuron, const std::vector<double> & inputs) {
    return activationFn(neuronPotential(neuron, inputs));
}

double BackpropagationNetwork::neuronPotential(const Neuron & neuron, const std::vector<double> & inputs) {

    double potential = 0.0;

    for (size_t i = 0; i < neuron.weights.size(); ++i) {
        potential += neuron.weights[i] * inputs[i];
    }

    return potential;
}

std::vector<double> BackpropagationNetwork::calcLayerValues(size_t layer, const std::vector<double> & inputs) {

    const auto & current = layers[layer];
    std::vector<double> out(current.size(), 0.0);

    for (size_t i = 0; i < current.size(); ++i) {
        out[i] = neuronOutput(current[i], inputs);
    }

    return out;
}

std::vector<double> BackpropagationNetwork::outputValues(std::vector<double> input) {
    
    for (size_t i = 1; i < layers.size(); ++i) {
        auto next = calcLayerValues(i, input);        
        input = std::move(next);
    }

    return input;
}

std::vector<std::vector<double>> BackpropagationNetwork::outputsOfAllLayers(const std::vector<double> & inputs) {

    std::vector<std::vector<double>> outputs;
    outputs.reserve(layers.size());

    outputs.emplace_back(calcLayerValues(1, inputs));

    for (size_t i = 2; i < layers.size(); ++i) {
        outputs.emplace_back(calcLayerValues(i, outputs[i-2]));
    }

    return outputs;
}

double BackpropagationNetwork::calcNetworkError(const std::vector<double> & signals, const size_t expected) {

    double error = 0;
    
    for (size_t i = 0; i < signals.size(); ++i) {
        const double expVal = (i == expected);
        const double received = signals[i];
        const double err = expected - received;
        error += err*err;
    }

    return error;
}

std::vector<double> BackpropagationNetwork::adjustOutputLayer(const std::vector<double> & inputs, const std::vector<double> & outputs, const size_t expected) {

    std::vector<double> errors;
    
    for (size_t i = 0; i < outputs.size(); ++i) {
        const double expVal = (i == expected);
        const double received = outputs[i];
        const double error = (expected - received) * lambda * received * (1 - received);
        errors.emplace_back(error);

        Neuron & neuron = layers.back()[i];
        for (size_t j = 0; j < neuron.weights.size(); ++j) {
            neuron.weights[j] += eta * error * inputs[j];
        }
    }

    return errors;
}

std::vector<double> BackpropagationNetwork::adjustHiddenLayer(const std::vector<double> & inputs, const std::vector<double> & outputs, std::vector<Neuron> & layer, const std::vector<double> & prevErrors, const size_t expected) {

    std::vector<double> errors;

    for (size_t i = 0; i < outputs.size(); ++i) {
        const double expVal = (i == expected);
        const double received = outputs[i];
        // TODO:
        const double error = (expected - received) * lambda * received * (1 - received);
        errors.emplace_back(error);

        Neuron & neuron = layers.back()[i];
        for (size_t j = 0; j < neuron.weights.size(); ++j) {
            neuron.weights[j] += eta * error * inputs[j];
        }
    }

    return errors;
}

bool BackpropagationNetwork::teachIteration(const std::vector<double> & signals, const size_t expected) {

    auto outputs = outputsOfAllLayers(signals);
    const auto & outputLayer = outputs.back();

    const double error = calcNetworkError(signals, expected);

    if (error < threshold) {
        return true;
    }

    return false;
}

void BackpropagationNetwork::teach(const std::vector<double> & signals, const size_t expected) {
    for (
        int i = 0;
        i < iterations and not teachIteration(signals, expected);
        ++i
    );
}


