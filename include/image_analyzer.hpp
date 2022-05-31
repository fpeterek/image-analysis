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
#include "filters.hpp"


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

    uint32_t id;
    Bounds bounds;
    std::uint32_t type = noType;
};

std::vector<Object> extractObjects(const Image & img);

template <std::uint32_t objects, typename ThresholdProvider>
class ImageAnalyzer {

    static constexpr int minObjectSize = 15;

    Thresholder<ThresholdProvider> tc;
    Indexer idx;
    Recognizer<objects> recognizer;
    sf::Font font;

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

    std::vector<signals::ObjectSignals> calcSignals(const Image & img, const int flags);

    void annotateObjects(const Image & img, const std::vector<Object> & obj, const int flags, const std::string filename);

    void reconstructIfDesired(const Image & img, const int flags, const std::string filename);
    void annotateObjectsIfDesired(const Image & img, const std::vector<Object> & obj, const int flags, const std::string filename);

    void recognizeObjects(const std::vector<signals::ObjectSignals> & signals, std::vector<Object> & obj);

public:

    struct Flags {
        const static int none = 0;
        const static int surfaceRecognition = 1;
        const static int annotateRecognized = 2;

        /* Shortcuts */
        const static int sr = surfaceRecognition;
        const static int ar = annotateRecognized;
    };

    ImageAnalyzer();

    void learn(const sf::Image & img, const int flags = Flags::sr | Flags::ar);
    void learn(const std::string & filename, const int flags = Flags::sr | Flags::ar);

    std::vector<Object> recognize(const sf::Image & img, const int flags = Flags::sr | Flags::ar);
    std::vector<Object> recognize(const std::string & filename, const int flags = Flags::sr | Flags::ar);

};


template <std::uint32_t objects, typename ThresholdProvider>
ImageAnalyzer<objects, ThresholdProvider>::ImageAnalyzer() {
    if (not font.loadFromFile("resources/Inconsolata-Regular.ttf")) {
        throw std::runtime_error("Font could not be loaded.");
    }
}

template <std::uint32_t objects, typename ThresholdProvider>
void ImageAnalyzer<objects, ThresholdProvider>::reconstructIfDesired(const Image & img, const int flags, const std::string file) {
    if (flags & Flags::surfaceRecognition) {
        img.reconstruct(colors).saveToFile(file);
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
std::vector<signals::ObjectSignals> ImageAnalyzer<objects, ThresholdProvider>::calcSignals(const Image & img, const int flags) {

    const auto signalMap = signals::getSignals(img);
    
    std::vector<signals::ObjectSignals> sigVec(signalMap.size());

    for (const auto & [idx, sig] : signalMap) {
        sigVec[idx] = sig;
    }

    return sigVec;
}


template <std::uint32_t objects, typename ThresholdProvider>
void ImageAnalyzer<objects, ThresholdProvider>::learn(const sf::Image & img, const int flags) {

    const auto thresholds = tc.findThresholds(img);
    const auto indexed = idx.assignIndices(thresholds);
    const auto filtered = filterBySize(indexed, minObjectSize);
    reconstructIfDesired(filtered, flags, "learning.reconstructed.png");

    const auto sigVec = calcSignals(filtered, flags);
    const auto clusters = KMeans<3>().cluster(sigVec);

    recognizer.learn(clusters);
}

template <std::uint32_t objects, typename ThresholdProvider>
void ImageAnalyzer<objects, ThresholdProvider>::recognizeObjects(const std::vector<signals::ObjectSignals> & signals, std::vector<Object> & obj) {

    for (const auto & sig : signals) {
        std::cout << sig.index << " " << sig.momentOfInertia << " " << sig.perimeterAreaRatio << std::endl;
        obj[sig.index].type = recognizer.recognize(sig);
    }

}

template <std::uint32_t objects, typename ThresholdProvider>
std::vector<Object> ImageAnalyzer<objects, ThresholdProvider>::recognize(const sf::Image & img, const int flags) {

    const auto thresholds = tc.findThresholds(img);
    const auto indexed = idx.assignIndices(thresholds);
    const auto filtered = filterBySize(indexed, minObjectSize);
    reconstructIfDesired(filtered, flags, "recognition.reconstructed.png");

    const auto sigVec = calcSignals(filtered, flags);
    auto objectVec = extractObjects(filtered);

    recognizeObjects(sigVec, objectVec);

    annotateObjectsIfDesired(indexed, objectVec, flags, "recognition.objects.png");

    return objectVec;
}

template <std::uint32_t objects, typename ThresholdProvider>
std::vector<Object> ImageAnalyzer<objects, ThresholdProvider>::recognize(const std::string & filename, const int flags) {
    sf::Image img;
    if (not img.loadFromFile(filename)) {
        throw std::runtime_error("File " + filename + " not found");
    }

    return recognize(img, flags);
}


template <std::uint32_t objects, typename ThresholdProvider>
void ImageAnalyzer<objects, ThresholdProvider>::annotateObjects(const Image & img, const std::vector<Object> & obj, const int flags, const std::string file) {
    auto rec = img.reconstruct(colors);

    sf::RenderTexture tex;
    tex.create(rec.getSize().x, rec.getSize().y);

    {
        sf::Texture texture;
        texture.loadFromImage(rec);

        tex.draw(sf::Sprite { texture });
    }

    for (const auto & o : obj) {
        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(30);
        text.setFillColor(sf::Color::White);
        text.setString(std::to_string(o.type));
        text.setOutlineThickness(2);
        text.setOutlineColor(sf::Color::Black);

        text.setOrigin(text.getGlobalBounds().width / 2.f, text.getGlobalBounds().height / 2.f);
        text.setPosition(
            o.bounds.leftTop.x + (o.bounds.rightBottom.x - o.bounds.leftTop.x) / 2.f,
            o.bounds.leftTop.y + (o.bounds.rightBottom.y - o.bounds.leftTop.y) / 2.f
        );
        tex.draw(text);
    }

    sf::Image dest = tex.getTexture().copyToImage();

    // For some reason, the output image is flipped, and thus has to be flipped
    // once more to obtain the proper orientation
    dest.flipVertically();

    dest.saveToFile(file);
}

template <std::uint32_t objects, typename ThresholdProvider>
void ImageAnalyzer<objects, ThresholdProvider>::annotateObjectsIfDesired(const Image & img, const std::vector<Object> & obj, const int flags, const std::string file) {
    if (flags & Flags::annotateRecognized) {
        annotateObjects(img, obj, flags, file);
    }
}

#endif

