#ifndef IMAGE_ANALYSIS_IMAGE_HPP
#define IMAGE_ANALYSIS_IMAGE_HPP

#include <vector>

#include <SFML/Graphics.hpp>

#include "pixel.hpp"


class Image {

    std::vector<std::vector<Pixel>> img;


public:

    Image(uint32_t width, uint32_t height);
    Image(std::vector<std::vector<Pixel>> img);
    Image(const Image & img);
    Image(Image && img);
    Image() noexcept = default;

    Image & operator=(const Image & img);
    Image & operator=(Image && img);

    sf::Vector2u size() const;
    uint32_t width() const;
    uint32_t height() const;

    const Pixel & at(uint32_t x, uint32_t y) const;
    Pixel & at(uint32_t x, uint32_t y);

    sf::Image reconstruct(const std::vector<sf::Color> & colors) const;

};

#endif

