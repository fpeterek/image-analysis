#include "thresholder.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <iostream>
#include <utility>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>


static std::pair<uint8_t, uint8_t> findMinMax(const sf::Image & src) {
    uint8_t min = 255;
    uint8_t max = 0;

    for (uint32_t y = 0; y < src.getSize().y; ++y) {
        for (uint32_t x = 0; x < src.getSize().x; ++x) {
            const auto current = util::bw(src.getPixel(x, y));
            
            min = std::min(min, current);
            max = std::max(max, current);
        }
    }

    return { min, max };
}


uint8_t HalfRangeThreshold::findThreshold(const sf::Image & img) {
    const auto [min, max] = findMinMax(img);
    return min + (max-min)/2;
}

