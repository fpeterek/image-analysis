#ifndef IMAGE_ANALYSIS_RECOGNITION_HPP
#define IMAGE_ANALYSIS_RECOGNITION_HPP

#include <unordered_map>
#include <vector>
#include <array>
#include <cstdint>

#include "signals.hpp"

struct Centroid {
    double perimeterAreaRatio = 0;
    double momentOfInertia    = 0;
    std::uint32_t objects     = 0;
};

class RecognizerUtil {
public:
    static Centroid calculateCentroid(const Centroid & origCentroid, const std::vector<signals::ObjectSignals> & cluster);
    static double calcDistance(const Centroid & centroid, const signals::ObjectSignals & cluster);
};

template <std::uint32_t objects>
class Recognizer {

    std::array<Centroid, objects> centroids;

public:

    Recognizer() = default;
    Recognizer(const std::array<std::vector<signals::ObjectSignals>, objects> & initialClusters);

    /* Incremental learning */
    void learn(const std::array<std::vector<signals::ObjectSignals>, objects> & clusters);
    void learn(const std::vector<signals::ObjectSignals> & signals, std::uint32_t cluster);
    void learn(const signals::ObjectSignals & signals, std::uint32_t cluster);

    std::uint32_t recognize(const signals::ObjectSignals & signals);
    std::vector<std::uint32_t> recognize(const std::vector<signals::ObjectSignals> & signals);

};

template <std::uint32_t objects>
Recognizer<objects>::Recognizer(const std::array<std::vector<signals::ObjectSignals>, objects> & initialClusters) {
    learn(initialClusters);
}

template <std::uint32_t objects>
void Recognizer<objects>::learn(const std::array<std::vector<signals::ObjectSignals>, objects> & toLearn) {

    for (size_t i = 0; i < toLearn.size(); ++i) {
        centroids[i] = RecognizerUtil::calculateCentroid(centroids[i], toLearn[i]);
    }

}

template <std::uint32_t objects>
void Recognizer<objects>::learn(const std::vector<signals::ObjectSignals> & toLearn, std::uint32_t cluster) {
    centroids[cluster] = RecognizerUtil::calculateCentroid(centroids[cluster], toLearn);
}

template <std::uint32_t objects>
void Recognizer<objects>::learn(const signals::ObjectSignals & toLearn, std::uint32_t cluster) {
    centroids[cluster].emplace_back(toLearn);
}

template <std::uint32_t objects>
std::uint32_t Recognizer<objects>::recognize(const signals::ObjectSignals & signals) {

    std::uint32_t minDistIdx;
    double minDist = -1;
    
    for (std::uint32_t i = 0; i < objects; ++i) {
        const auto dist = RecognizerUtil::calcDistance(centroids[i], signals);

        if (minDist < 0 or dist < minDist) {
            minDist = dist;
            minDistIdx = i;
        }
    }
    return minDistIdx; 
}

template <std::uint32_t objects>
std::vector<std::uint32_t> Recognizer<objects>::recognize(const std::vector<signals::ObjectSignals> & signals) {

    std::vector<std::uint32_t> res;
    res.reserve(signals.size());

    for (const auto & sig : signals) {
        res.emplace_back(recognize(sig));
    }

    return res;
}

#endif

