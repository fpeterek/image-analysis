#include "image.hpp"

#include <SFML/Graphics/Color.hpp>
#include <cstdint>
#include <algorithm>
#include <functional>
#include <stdexcept>

#include <SFML/System.hpp>


Image::Image(const uint32_t width, const uint32_t height) : img(height) {

    if (height and not width) {
        throw std::runtime_error("Image must be empty at least one pixel wide");
    }

    if (width and not height) {
        throw std::runtime_error("Image must be empty at least one pixel tall");
    }

    auto genLine = [width]() -> auto {
        std::vector<Pixel> pixels(width);
        std::generate(pixels.begin(), pixels.end(), []() -> auto { return Pixel(); });

        return pixels;
    };

    std::generate(img.begin(), img.end(), genLine);

}

Image::Image(std::vector<std::vector<Pixel>> img) : img(std::move(img)) {
    if (not img.empty()) {
        const auto refSize = img.front().size();

        if (not refSize) {
            throw std::runtime_error("Image must be at least one pixel wide");
        }

        for (const auto & vec : img) {
            if (vec.size() != refSize) {
                throw std::runtime_error("All image rows must be of equal size");
            }
        }
    }

}

Image::Image(const Image & img) : img(img.img) { }

Image::Image(Image && img) : img(std::move(img.img)) { }

Image & Image::operator=(const Image & other) {
    img = other.img;
    return *this;
}

Image & Image::operator=(Image && other) {
    img = std::move(other.img);
    return *this;
}

sf::Vector2u Image::size() const {
    return {
        (std::uint32_t)img.front().size(),
        (std::uint32_t)img.size()
    };
} 

uint32_t Image::width() const {
    return img.front().size();
}

uint32_t Image::height() const {
    return img.size();
}

const Pixel & Image::at(const uint32_t x, const uint32_t y) const {
    return img[y][x];
}

Pixel & Image::at(const uint32_t x, const uint32_t y) {
    return img[y][x];
}

sf::Image Image::reconstruct(const std::vector<sf::Color> & colors) const {
    sf::Image img;
    img.create(width(), height(), sf::Color::Black);

    for (uint32_t y = 0; y < height(); ++y) {
        for (uint32_t x = 0; x < width(); ++x) {
            auto & px = at(x, y);
            
            if (px.isIndexed()) {
                img.setPixel(x, y, colors[px.index % colors.size()]);
            }
        }
    }

    return img;
}

