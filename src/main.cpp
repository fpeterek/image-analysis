#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

#include "image.hpp"
#include "thresholder.hpp"
#include "indexer.hpp"
#include "signals.hpp"
#include "kmeans.hpp"
#include "filters.hpp"

namespace sig = signals;

int main(int argc, const char * argv[]) {
    sf::Image in;
    in.loadFromFile("resources/train/train.bmp");
    
    /* Due to the simple nature of the input image, we can fine tune the threshold */
    /* manually to suit our needs                                                  */
    Thresholder<ConstantThreshold<35>> th;
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

    const auto filtered = filterBySize(indexed, 10);
    std::cout << "Small objects filtered" << std::endl;

    const auto recon = filtered.reconstruct(colors);
    std::cout << "Image reconstructed" << std::endl;

    recon.saveToFile("indexed.png");

    const auto signals = sig::getSignals(filtered);
    std::vector<sig::ObjectSignals> sigVec;

    for (const auto & [idx, sig] : signals) {
        std::cout << "Object " << idx << " { perimeterAreaRatio: " << sig.perimeterAreaRatio
                  << ", momentOfInertia: " << sig.momentOfInertia << " }\n";
        sigVec.emplace_back(sig);
    }
    std::flush(std::cout);

    const auto clusters = KMeans<3>().cluster(sigVec);

    for (size_t i = 0; i < 3; ++i) {
        const auto & cluster = clusters[i];

        std::cout << "Cluster " << (i+1) << "\n\n";

        for (const auto & s : cluster) {
            std::cout << s.index << " { momentOfInertia: " << s.momentOfInertia << ", perimeterAreaRatio: " << s.perimeterAreaRatio
                      << " }" << std::endl;
        }

        std::cout << "\n" << std::endl;
    }

}

