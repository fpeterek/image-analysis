#include "recognition.hpp"

#include <cmath>


namespace recognizerUtil {

    Centroid calculateCentroid(const Centroid & left, const Centroid & right) {
        if (not (left.objects or right.objects)) {
            return { };
        }

        const auto totalObjects = left.objects + right.objects;
        const auto origWeight = left.objects / (double)totalObjects;
        const auto additionWeight = right.objects / (double)totalObjects;

        return {
            left.perimeterAreaRatio * origWeight + right.perimeterAreaRatio * additionWeight,
            left.momentOfInertia * origWeight + right.momentOfInertia * additionWeight,
            (std::uint32_t)totalObjects
        };
    }

    Centroid calculateCentroid(const Centroid & origCentroid, const std::vector<signals::ObjectSignals> & cluster) {
        return calculateCentroid(origCentroid, calculateCentroid(cluster));
    }


    Centroid calculateCentroid(const std::vector<signals::ObjectSignals> & cluster) {

        if (cluster.empty()) {
            return {};
        }

        double paTotal = 0;
        double moiTotal = 0;

        const double totalObjects = cluster.size();

        for (const auto & sig : cluster) {
            paTotal += sig.perimeterAreaRatio;        
            moiTotal += sig.momentOfInertia;
        }

        return {
            paTotal / totalObjects,
            moiTotal / totalObjects,
            (std::uint32_t)cluster.size()
        };
    }

    double calcDistance(const Centroid & centroid, const signals::ObjectSignals & cluster) {
        const double dx = centroid.perimeterAreaRatio - cluster.perimeterAreaRatio;
        const double dy = centroid.momentOfInertia - cluster.momentOfInertia;

        return std::sqrt(dx*dx + dy*dy);
    }

}

