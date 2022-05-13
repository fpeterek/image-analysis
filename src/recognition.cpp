#include "recognition.hpp"

#include <cmath>


Centroid RecognizerUtil::calculateCentroid(const Centroid & origCentroid, const std::vector<signals::ObjectSignals> & cluster) {

    if (cluster.empty()) {
        return origCentroid;
    }

    double paRatio = 0;
    double moi = 0;

    const auto totalObjects = origCentroid.objects + cluster.size();
    const auto origWeight = origCentroid.objects / (double)totalObjects;
    const auto additionWeight = cluster.size() / (double)totalObjects;

    for (const auto & sig : cluster) {
        paRatio += sig.perimeterAreaRatio;        
        moi += sig.momentOfInertia;
    }

    return {
        origCentroid.perimeterAreaRatio * origWeight + paRatio * additionWeight,
        origCentroid.momentOfInertia * origWeight + moi * additionWeight,
        (std::uint32_t)totalObjects
    };
}

double RecognizerUtil::calcDistance(const Centroid & centroid, const signals::ObjectSignals & cluster) {
    const double dx = centroid.perimeterAreaRatio - cluster.perimeterAreaRatio;
    const double dy = centroid.momentOfInertia - cluster.momentOfInertia;

    return std::sqrt(dx*dx + dy*dy);
}

