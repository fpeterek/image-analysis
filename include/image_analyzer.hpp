#ifndef IMAGE_ANALYSIS_IMAGE_ANALYZER_HPP
#define IMAGE_ANALYSIS_IMAGE_ANALYZER_HPP

#include <vector>
#include <stdexcept>
#include <iostream>

#include <SFML/Graphics.hpp>

#include "indexer.hpp"
#include "signals.hpp"
#include "recognition.hpp"
#include "kmeans.hpp"
#include "thresholder.hpp"


struct Point {
    uint32_t x;
    uint32_t y;
};

struct Bounds {
    Point leftTop;
    Point rightBottom;
};

struct Object {
    static constexpr std::uint32_t noType = -1;

    Bounds bounds;
    std::uint32_t type = noType;
};


template <std::uint32_t objects, typename ThresholdProvider>
class ImageAnalyzer {

    static constexpr int minObjectSize = 15;

    Thresholder<ThresholdProvider> tc;
    Indexer idx;
    Recognizer<objects> recognizer;

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

    void reconstructIfDesired(const Image & img, const int flags);

public:

    struct Flags {
        const static int surfaceRecognition = 1;
        const static int annotateRecognized = 2;

        /* Shortcuts */
        const static int sr = surfaceRecognition;
        const static int ar = annotateRecognized;
    };

    void learn(const sf::Image & img, const int flags = Flags::sr | Flags::ar);
    void learn(const std::string & filename, const int flags = Flags::sr | Flags::ar);

    std::vector<Object> recognize(const sf::Image & img, const int flags = Flags::sr | Flags::ar);

};

template <std::uint32_t objects, typename ThresholdProvider>
void ImageAnalyzer<objects, ThresholdProvider>::reconstructIfDesired(const Image & img, const int flags) {
    if (flags & Flags::surfaceRecognition) {
        img.reconstruct(colors).saveToFile("reconstructed.png");
    }
}

template <std::uint32_t objects, typename ThresholdProvider>
void ImageAnalyzer<objects, ThresholdProvider>::learn(const std::string & filename, const int flags) {

    sf::Image img;
    if (not img.loadFromFile(filename)) {
        throw std::runtime_error("File could not be loaded");
    }

    learn(img, flags);
}

template <std::uint32_t objects, typename ThresholdProvider>
void ImageAnalyzer<objects, ThresholdProvider>::learn(const sf::Image & img, const int flags) {

    const auto thresholds = tc.findThresholds(img);
    const auto indexed = idx.assignIndices(thresholds);
    
    const auto filtered = filterBySize(indexed, minObjectSize);
    reconstructIfDesired(filtered, flags);

    const auto signalMap = signals::getSignals(filtered);
    std::cout << "Signals computed" << std::endl;
    
    std::vector<signals::ObjectSignals> sigVec(signalMap.size());

    for (const auto & [idx, sig] : signalMap) {
        sigVec[idx] = sig;
    }
    std::cout << "Vector created" << std::endl;

    const auto clusters = KMeans<3>().cluster(sigVec);
    std::cout << "Clustering performed" << std::endl;

    recognizer.learn(clusters);
}



#endif

