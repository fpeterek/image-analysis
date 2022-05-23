#ifndef IMAGE_ANALYSIS_KMEANS_HPP
#define IMAGE_ANALYSIS_KMEANS_HPP

#include <cstdint>
#include <limits>
#include <optional>
#include <vector>
#include <array>
#include <random>
#include <iostream>

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

bool operator==(km::Centroid c1, km::Centroid c2);
bool operator!=(km::Centroid c1, km::Centroid c2);

template <uint64_t clusters>
class KMeans {

    std::mt19937 mt { std::random_device()() };

    std::array<km::Centroid, clusters> randCentroids(const km::Signals & signals);
    std::array<km::Signals, clusters> distribute(const km::Signals & signals, const std::array<km::Centroid, clusters> & centroids);
    std::array<km::Centroid, clusters> calcCentroids(const std::array<km::Signals, clusters> & signals);

    double calcDistSse(const std::array<km::Signals, clusters> & signals, const std::array<km::Centroid, clusters> & centroids);

    std::pair<std::array<km::Centroid, clusters>, std::array<km::Signals, clusters>> runIter(const km::Signals & signals);

    bool containsEmptyCluster(const std::array<km::Signals, clusters> & distribution);

public:

    std::array<std::vector<signals::ObjectSignals>, clusters> cluster(const std::vector<signals::ObjectSignals> & signals, const int attempts = 10);

};

template <uint64_t clusters>
std::array<km::Centroid, clusters> KMeans<clusters>::randCentroids(const km::Signals & signals) {

    std::vector<size_t> indices(signals.size());
    std::iota(indices.begin(), indices.end(), 0);

    std::shuffle(indices.begin(), indices.end(), mt);

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

        /* std::cout << sig.momentOfInertia << ", " << sig.perimeterAreaRatio << " Before emplacing at idx " << minCentroid << std::endl; */
        /* for (const auto & s : dist[minCentroid]) { */
        /*     std::cout << s.momentOfInertia << ", " << s.perimeterAreaRatio << std::endl; */
        /* } */
        /* std::cout << "total size: " << dist[minCentroid].size() << std::endl; */
        dist[minCentroid].emplace_back(sig);
        /* std::cout << "Post emplace" << std::endl; */
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
            sumX += signal.perimeterAreaRatio;
            sumY += signal.momentOfInertia;
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
bool arrEq(const std::array<km::Centroid, clusters> & a1, const std::array<km::Centroid, clusters> & a2) {
    auto i1 = a1.begin();
    auto i2 = a2.begin();

    while (i1 != a1.end()) {
        if (*(i1++) != *(i2++)) {
            return false;
        }
    }
    return true;
}

template <uint64_t clusters>
std::pair<std::array<km::Centroid, clusters>, std::array<km::Signals, clusters>> KMeans<clusters>::runIter(const km::Signals & signals) {

    auto centroids = randCentroids(signals);
    std::array<km::Signals, clusters> result;

    for (int i = 0; i < km::maxKMIterations; ++i) {
        std::cout << "Pre Iter distribution" << std::endl;
        result = distribute(signals, centroids);
        std::cout << "Iter distribution" << std::endl;
        auto newCentroids = calcCentroids(result);
        std::cout << "Centroids calculated" << std::endl;

        // G++ appears to dislike the commented out code and thus I have to implement my own way
        // of comparing arrays
        /* if (newCentroids == centroids) { */
        /*     break; */
        /* } */
        if (arrEq(newCentroids, centroids)) {
            break;
        }

        centroids = newCentroids;
    }

    return std::pair { centroids, result };
}

template <uint64_t clusters>
bool KMeans<clusters>::containsEmptyCluster(const std::array<km::Signals, clusters> & distribution) {
    for (const auto & cluster : distribution) {
        if (cluster.empty()) {
            return true;
        }
    }
    return false;
}

template <uint64_t clusters>
std::array<std::vector<signals::ObjectSignals>, clusters> KMeans<clusters>::cluster(const km::Signals & signals, const int attempts) {

    double bestSse = -1;
    std::array<std::vector<signals::ObjectSignals>, clusters> bestIter;

    for (int i = 0; i < attempts; ++i) {
        
        std::cout << "Running iteration" << std::endl;
        auto [centroids, distribution] = runIter(signals);

        if (containsEmptyCluster(distribution)) {
            continue;
        }

        std::cout << "Iteration performed" << std::endl;
        const auto sse = calcDistSse(distribution, centroids);
        std::cout << "SSE calculated" << std::endl;

        if (bestSse == -1 or sse > bestSse) {
            bestSse = sse;
            bestIter = std::move(distribution);
        }
        std::cout << "Moved" << std::endl;
    }

    if (bestSse < 0) {
        throw std::runtime_error("Clustering failed (this should not have happened)");
    }

    return bestIter;
}


#endif

