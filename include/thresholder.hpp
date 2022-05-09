#ifndef IMAGE_ANALYSIS_THRESHOLDER_HPP
#define IMAGE_ANALYSIS_THRESHOLDER_HPP

#include <SFML/Graphics/Image.hpp>
#include <utility>
#include <functional>
#include <optional>

#include <SFML/Graphics.hpp>

#include "image.hpp"
#include "util.hpp"


struct HalfRangeThreshold {
    uint8_t findThreshold(const sf::Image & img);
};

template <uint8_t threshold>
struct ConstantThreshold {
    uint8_t findThreshold(const sf::Image & img);
};

template <uint8_t threshold>
uint8_t ConstantThreshold<threshold>::findThreshold(const sf::Image &) {
    return threshold;
}


template<typename ThresholdCalculator>
class Thresholder {

    ThresholdCalculator tc;

    Image dest;
    std::optional<std::reference_wrapper<const sf::Image>> src;

    void performThresholding();

public:

    Image findThresholds(const sf::Image & img);

};

template<typename TC>
void Thresholder<TC>::performThresholding() {
    const auto th = tc.findThreshold(src.value().get());
    
    for (uint32_t y = 0; y < dest.height(); ++y) {
        for (uint32_t x = 0; x < dest.width(); ++x) {
            const auto current = util::bw(src.value().get().getPixel(x, y));

            dest.at(x, y).color = (current <= th) ? Pixel::colorMin : Pixel::colorMax;
        }
    }
}

template<typename TC>
Image Thresholder<TC>::findThresholds(const sf::Image & img) {

    dest = Image(img.getSize().x, img.getSize().y);
    src = img;

    performThresholding();

    return std::move(dest);
}

#endif

