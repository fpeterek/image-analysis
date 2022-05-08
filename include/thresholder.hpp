#ifndef IMAGE_ANALYSIS_THRESHOLDER_HPP
#define IMAGE_ANALYSIS_THRESHOLDER_HPP

#include "image.hpp"

#include <utility>
#include <functional>
#include <optional>

#include <SFML/Graphics.hpp>

class Thresholder {

    Image dest;
    std::optional<std::reference_wrapper<const sf::Image>> src;

    uint8_t bw(sf::Color px);
    std::pair<uint8_t, uint8_t> findMinMax();
    uint8_t findThreshold();

    void performThresholding();

public:

    Image findThresholds(const sf::Image & img);

};

#endif

