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
    static Centroid calculateCentroid(const std::vector<signals::ObjectSignals> & cluster);
    static Centroid calculateCentroid(const Centroid & origCentroid, const std::vector<signals::ObjectSignals> & cluster);
    static Centroid calculateCentroid(const Centroid & left, const Centroid & right);
    static double calcDistance(const Centroid & centroid, const signals::ObjectSignals & cluster);
};

template <std::uint32_t objects>
class Recognizer {

    std::array<Centroid, objects> centroids;

    std::uint32_t findClosestCentroid(const Centroid & centroid);

public:

    Recognizer() = default;
    Recognizer(const std::array<std::vector<signals::ObjectSignals>, objects> & initialClusters);
    Recognizer(const std::array<Centroid, objects> & centroids);
    Recognizer(std::array<Centroid, objects> centroids);

    /* Incremental learning */
    void learn(const std::array<std::vector<signals::ObjectSignals>, objects> & clusters);
    void learn(const std::vector<signals::ObjectSignals> & signals);
    void learn(const signals::ObjectSignals & signals);

    std::uint32_t recognize(const signals::ObjectSignals & signals);
    std::vector<std::uint32_t> recognize(const std::vector<signals::ObjectSignals> & signals);

    bool untrained();

};


template <std::uint32_t objects>
bool Recognizer<objects>::untrained() {
    for (std::uint32_t i = 0; i < objects; ++i) {
        if (centroids[i].objects) {
            return false;
        }
    }
    return true;
}

template <std::uint32_t objects>
Recognizer<objects>::Recognizer(const std::array<Centroid, objects> & ct) : centroids(ct) { }

template <std::uint32_t objects>
Recognizer<objects>::Recognizer(std::array<Centroid, objects> ct) : centroids(std::move(ct)) { }
 
template <std::uint32_t objects>
Recognizer<objects>::Recognizer(const std::array<std::vector<signals::ObjectSignals>, objects> & initialClusters) {
    learn(initialClusters);
}

template <std::uint32_t objects>
void Recognizer<objects>::learn(const std::array<std::vector<signals::ObjectSignals>, objects> & toLearn) {

    for (const auto & vector : toLearn) {
        learn(vector);
    }

}


template <std::uint32_t objects>
std::uint32_t Recognizer<objects>::findClosestCentroid(const Centroid & centroid) {
    // Reuse the code we already have, there's no need to reinvent the wheel
    return recognize(signals::ObjectSignals {
        0,
        centroid.perimeterAreaRatio,
        centroid.momentOfInertia
    });
}

template <std::uint32_t objects>
void Recognizer<objects>::learn(const std::vector<signals::ObjectSignals> & toLearn) {
    const auto centroid = RecognizerUtil::calculateCentroid(toLearn);
    const auto closestIdx = findClosestCentroid(centroid);

    centroids[closestIdx] = RecognizerUtil::calculateCentroid(centroids[closestIdx], centroid);
}

template <std::uint32_t objects>
void Recognizer<objects>::learn(const signals::ObjectSignals & toLearn) {
    learn(std::vector<signals::ObjectSignals> { toLearn });
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

