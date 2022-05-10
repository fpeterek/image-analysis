#include "kmeans.hpp"


namespace km {
    double distance(const Centroid & centroid, const signals::ObjectSignals & signals) {

        const auto dx = centroid.x - signals.perimeterAreaRatio;
        const auto dy = centroid.y - signals.momentOfInertia;

        return std::sqrt(dx*dx + dy*dy);
    }
}

