#ifndef IMAGE_ANALYSIS_SIGNALS_HPP
#define IMAGE_ANALYSIS_SIGNALS_HPP

#include <unordered_map>
#include <cstdint>

#include "image.hpp"


namespace signals {

    struct ObjectSignals {
        uint32_t index;
        double perimeterAreaRatio;
        double momentOfInertia;
    };

    std::unordered_map<uint32_t, ObjectSignals> getSignals(const Image & img);

    std::unordered_map<uint32_t, double> getPerimeters(const Image & img);
}


#endif

