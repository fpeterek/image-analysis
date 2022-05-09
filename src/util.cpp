#include "util.hpp"


namespace util {
    uint8_t bw(const sf::Color px) {
        return px.r * 0.299 + px.g * 0.587 + px.b * 0.114;
    }
}

