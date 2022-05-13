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


int main(int argc, const char * argv[]) {
    
    /* Due to the simple nature of the input image, we can fine tune the threshold */
    /* manually to suit our needs                                                  */
    ImageAnalyzer<3, ConstantThreshold<35>> analyzer;

    analyzer.learn("resources/train/train.bmp");

}

