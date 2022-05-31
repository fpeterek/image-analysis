#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

#include "image.hpp"
#include "thresholder.hpp"
#include "indexer.hpp"
#include "signals.hpp"
#include "kmeans.hpp"
#include "filters.hpp"
#include "image_analyzer.hpp"

void run() {

    /* Due to the simple nature of the input image, we can fine tune the threshold */
    /* manually to suit our needs                                                  */
    ImageAnalyzer<3, ConstantThreshold<35>> analyzer;

    analyzer.learn("resources/train/train.bmp");
    analyzer.recognize("resources/test/test.bmp");
}

int main(int argc, const char * argv[]) {
    try {
        run();
    } catch (const std::exception & e) {
        std::cout << e.what() << std::endl;
    }
}

