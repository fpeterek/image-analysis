#ifndef IMAGE_ANALYSIS_KMEANS_HPP
#define IMAGE_ANALYSIS_KMEANS_HPP

#include <cstdint>
#include <limits>
#include <vector>
#include <array>
#include <random>

#include "signals.hpp"

namespace km {

    struct Centroid {
        double x;
        double y;
    };

    typedef std::vector<signals::ObjectSignals> Signals;

    double distance(const Centroid & centroid, const signals::ObjectSignals & signals);

    constexpr int maxKMIterations = 10;
}

template <uint64_t clusters>
class KMeans {

    std::random_device random;


    std::array<km::Centroid, clusters> randCentroids(const km::Signals & signals);
    std::array<km::Signals, clusters> distribute(const km::Signals & signals, const std::array<km::Centroid, clusters> & centroids);
    std::array<km::Centroid, clusters> calcCentroids(const std::array<km::Signals, clusters> & signals);

    double calcDistSse(const std::array<km::Signals, clusters> & signals, const std::array<km::Centroid, clusters> & centroids);

    std::pair<std::array<km::Centroid, clusters>, std::array<km::Signals, clusters>> runIter(const km::Signals & signals);

public:

    std::array<std::vector<signals::ObjectSignals>, clusters> cluster(const std::vector<signals::ObjectSignals> & signals, const int attempts = 10);

};

template <uint64_t clusters>
std::array<km::Centroid, clusters> KMeans<clusters>::randCentroids(const km::Signals & signals) {

    std::vector<size_t> indices(signals.size());
    std::iota(indices.begin(), indices.end(), 0);

    std::random_shuffle(indices.begin(), indices.end(), random);

    std::array<km::Centroid, clusters> centroids;

    for (uint64_t i = 0; i < clusters; ++i) {
        const auto & sig = signals[i];
        centroids[i] = { sig.perimeterAreaRatio, sig.momentOfInertia };
    }

    return centroids;
}

template <uint64_t clusters>
std::array<km::Signals, clusters> KMeans<clusters>::distribute(const km::Signals & signals, const std::array<km::Centroid, clusters> & centroids) {

     std::array<km::Signals, clusters> dist;

     for (const auto & sig : signals) {
        uint64_t minCentroid = 0;
        double minDistance = -1;

        for (size_t i = 0; i < clusters; ++i) {
            const auto dist = km::distance(centroids[i], sig);

            if (minDistance < 0 or dist < minDistance) {
                minDistance = dist;
                minCentroid = i;
            }
        }

        dist[minCentroid].emplace_back(sig);
     }

     return dist;
}

template <uint64_t clusters>
std::array<km::Centroid, clusters> KMeans<clusters>::calcCentroids(const std::array<km::Signals, clusters> & signals) {

    std::array<km::Centroid, clusters> centroids;

    for (uint64_t i = 0; i < clusters; ++i) {

        const auto & sigs = signals[i]; 

        double sumX = 0;
        double sumY = 0;

        for (const auto & signal : sigs) {
            sumX += signal.x;
            sumY += signal.y;
        }

        if (sigs.empty()) {
            centroids[i] = { std::numeric_limits<double>::max(), std::numeric_limits<double>::max() };
        } else {
            centroids[i] = { sumX / sigs.size(), sumY / sigs.size() };
        }
    }

    return centroids;
}

template <uint64_t clusters>
double KMeans<clusters>::calcDistSse(const std::array<km::Signals, clusters> & signals, const std::array<km::Centroid, clusters> & centroids) {
    double distances = 0;

    for (uint64_t i = 0; i < clusters; ++i) {
        const auto & centroid = centroids[i];
        const auto & cluster = signals[i];

        for (const auto & sig : cluster) {
            const auto dist = km::distance(centroid, sig);
            distances += dist * dist;
        }
    }

    return distances;
}

template <uint64_t clusters>
std::pair<std::array<km::Centroid, clusters>, std::array<km::Signals, clusters>> KMeans<clusters>::runIter(const km::Signals & signals) {

    auto centroids = randCentroids(signals);
    std::array<km::Signals, clusters> result;

    for (int i = 0; i < km::maxKMIterations; ++i) {
        result = distribute(centroids, signals);
        auto newCentroids = calcCentroids(result);

        if (newCentroids == centroids) {
            break;
        }

        centroids = std::move(newCentroids);
    }

    return std::pair { centroids, result };
}


template <uint64_t clusters>
std::array<std::vector<signals::ObjectSignals>, clusters> KMeans<clusters>::cluster(const km::Signals & signals, const int attempts) {

    double bestSse = -1;
    std::array<std::vector<signals::ObjectSignals>, clusters> bestIter;

    for (int i = 0; i < attempts; ++i) {
        
        auto [centroids, distribution] = runIter(signals);
        const auto sse = calcDistSse(distribution, centroids);

        if (bestSse == -1 or sse > bestSse) {
            bestSse = sse;
            bestIter = std::move(centroids);
        }
    }

    return bestIter;
}


#endif

