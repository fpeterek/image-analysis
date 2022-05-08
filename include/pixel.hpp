#ifndef IMAGE_ANALYSIS_PIXEL_HPP
#define IMAGE_ANALYSIS_PIXEL_HPP

#include <cstdint>


struct Pixel {

    static constexpr std::uint32_t noIndex = -1;

    static constexpr std::uint8_t colorMin = 0;
    static constexpr std::uint8_t colorMax = 255;

    std::uint8_t color = 0;
    std::uint32_t index = noIndex;

};

#endif


