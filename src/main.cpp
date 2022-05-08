#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

#include "image.hpp"
#include "thresholder.hpp"
#include "indexer.hpp"

int main(int argc, const char * argv[]) {
    sf::Image in;
    in.loadFromFile("resources/train/train.bmp");
    
    Thresholder th;
    Indexer idx;

    std::vector<sf::Color> colors {
        sf::Color(252, 186, 3),
        sf::Color(252, 244, 3),
        sf::Color(252, 65, 3),
        sf::Color(152, 252, 3),

        sf::Color(3, 252, 90),
        sf::Color(3, 252, 194),
        sf::Color(3, 227, 252),
        sf::Color(3, 157, 252),

        sf::Color(3, 65, 252),
        sf::Color(119, 3, 252),
        sf::Color(223, 3, 252),
        sf::Color(252, 3, 119)
    };

    const auto thresholds = th.findThresholds(in);
    std::cout << "Thresholds found" << std::endl;
    const auto indexed = idx.assignIndices(thresholds);
    std::cout << "Objects indexed" << std::endl;
    const auto recon = indexed.reconstruct(colors);
    std::cout << "Image reconstructed" << std::endl;

    recon.saveToFile("indexed.png");

}

