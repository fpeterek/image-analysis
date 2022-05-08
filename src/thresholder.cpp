#include "thresholder.hpp"

#include <SFML/Graphics/Color.hpp>
#include <iostream>
#include <utility>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>


uint8_t Thresholder::bw(const sf::Color px) {
    return px.r * 0.299 + px.g * 0.587 + px.b * 0.114;
}

std::pair<uint8_t, uint8_t> Thresholder::findMinMax() {
    uint8_t min = 255;
    uint8_t max = 0;

    for (uint32_t y = 0; y < dest.height(); ++y) {
        for (uint32_t x = 0; x < dest.width(); ++x) {
            const auto current = bw(src.value().get().getPixel(x, y));
            
            min = std::min(min, current);
            max = std::max(max, current);
        }
    }

    return { min, max };
}

/* Due to the nature of the input image, a manually selected threshold */
/* yields better results                                               */
uint8_t Thresholder::findThreshold() {
    const auto [min, max] = findMinMax();
    /* return min + (max - min)/2; */
    return 50;
}

void Thresholder::performThresholding() {
    const auto th = findThreshold();
    
    for (uint32_t y = 0; y < dest.height(); ++y) {
        for (uint32_t x = 0; x < dest.width(); ++x) {
            const auto current = bw(src.value().get().getPixel(x, y));

            dest.at(x, y).color = (current <= th) ? Pixel::colorMin : Pixel::colorMax;
        }
    }
}

Image Thresholder::findThresholds(const sf::Image & img) {

    dest = Image(img.getSize().x, img.getSize().y);
    src = img;

    performThresholding();

    return std::move(dest);
}

