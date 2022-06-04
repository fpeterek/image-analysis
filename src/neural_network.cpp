#include "neural_network.hpp"


Layer::Layer(size_t size) : layers(size) { }


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

    for (auto & neuron : layer.neurons) {
        neuron.weights.reserve(weigths);
        for (size_t i = 0; i < weigths; ++i) {
            neuron.weights.emplace_back(dist(rnd));
        }
    }
}

