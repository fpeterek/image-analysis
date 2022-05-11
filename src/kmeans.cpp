#include "kmeans.hpp"


namespace km {
    double distance(const Centroid & centroid, const signals::ObjectSignals & signals) {

        const auto dx = centroid.x - signals.perimeterAreaRatio;
        const auto dy = centroid.y - signals.momentOfInertia;

        return std::sqrt(dx*dx + dy*dy);
    }
}

bool operator==(const km::Centroid c1, const km::Centroid c2) {
    return c1.x == c2.x and c1.y == c2.y;
}

bool operator!=(const km::Centroid c1, const km::Centroid c2) {
    return not (c1 == c2);
}

