#include "signals.hpp"

#include <cmath>
#include <iostream>

namespace signals {


    typedef std::unordered_map<uint32_t, std::pair<uint32_t, uint32_t>> IntPairMap;
    typedef std::unordered_map<uint32_t, std::pair<double, double>> DoublePairMap;
    typedef std::unordered_map<uint32_t, double> SignalMap;

    SignalMap emptyMap() {
        return {};
    }

    template <typename T>
    std::unordered_map<uint32_t, std::pair<T, T>> emptyPairs() {
        return {};
    }

    void forAll(const Image & img, const std::function<void(uint32_t, uint32_t, const Pixel & px)> & fn) {

        for (uint32_t yc = 0; yc < img.height(); ++yc) {
            for (uint32_t xc = 0; xc < img.width(); ++xc) {
                const auto & px = img.at(xc, yc);
                fn(xc, yc, px);
            }
        }
    }

    auto moment(const Image & img, const int xExp, const int yExp) {

        auto moments = emptyMap();

        const auto fn = [&moments, xExp, yExp](const uint32_t x, const uint32_t y, const Pixel & px) {
            if (not px.isIndexed()) {
                return;
            }

            moments[px.index] += std::pow(x, xExp) * std::pow(y, yExp);
        };

        forAll(img, fn);

        return moments;
    }

    auto centerOfMass(const Image & img, const SignalMap & m0) {
        auto sig = emptyPairs<double>();

        const auto m10 = moment(img, 1, 0);
        const auto m01 = moment(img, 0, 1);

        for (const auto & [idx, m] : m0) {
            const auto xm = m10.at(idx);
            const auto ym = m01.at(idx);

            sig[idx] = { xm / (double)m, ym / double(m) };
        }

        return sig;
    }

    auto mu(const Image & img, const DoublePairMap & massCenter, const int xExp, const int yExp) {

        auto muMap = emptyMap();

        const auto fn = [&massCenter, &muMap, xExp, yExp](const uint32_t x, const uint32_t y, const Pixel & px) {
            if (not px.isIndexed()) {
                return;
            }

            const auto xcm = massCenter.at(px.index).first;
            const auto ycm = massCenter.at(px.index).second;

            muMap[px.index] += std::pow(x - xcm, xExp) + std::pow(y - ycm, yExp);
        };

        forAll(img, fn);

        return muMap;
    }

    auto circumference(const Image & img) {
        auto sig = emptyMap();

        const auto fn = [&sig, &img](const uint32_t x, const uint32_t y, const Pixel & px) {
            if (not px.isIndexed()) {
                return;
            }

            const bool left = (not x) or (img.at(x-1, y).index != px.index);
            const bool right = (x >= img.width() - 1) or (img.at(x+1, y).index != px.index);

            const bool up = (not y) or (img.at(x, y-1).index != px.index);
            const bool down = (y >= img.height() - 1) or (img.at(x, y+1).index != px.index);

            sig[px.index] += left or right or up or down;
        };

        forAll(img, fn);

        return sig;
    }

    SignalMap perimeterAreaRatio(const Image & img, const SignalMap & area) {
        auto sig = emptyMap();

        const auto circ = circumference(img);

        for (const auto & [idx, a] : area) {
            const auto c = circ.at(idx);

            sig[idx] = (c * c) / (100 * a);
        }

        return sig;
    }

    SignalMap momentOfInertia(const Image & img, const SignalMap & m0) {
        auto sig = emptyMap();

        const auto massCenter = centerOfMass(img, m0);
        const auto mu20 = mu(img, massCenter, 2, 0);
        const auto mu02 = mu(img, massCenter, 0, 2);
        const auto mu11 = mu(img, massCenter, 1, 1);

        for (const auto & [idx, m11] : mu11) {
            const auto m20 = mu20.at(idx);
            const auto m02 = mu02.at(idx);

            const auto left = 0.5 * (m20 + m02);
            const auto right = 0.5 * std::sqrt(4*m11*m11 + std::pow(m20 - m02, 2));

            const auto muMax = left + right;
            const auto muMin = left - right;

            sig[idx] = muMin / muMax;
        }

        return sig;
    }

    std::unordered_map<uint32_t, ObjectSignals> getSignals(const Image & img) {
        const auto m0 = moment(img, 0, 0);
        const auto par = perimeterAreaRatio(img, m0);
        const auto moi = momentOfInertia(img, m0);

        std::unordered_map<uint32_t, ObjectSignals> sig;

        for (const auto & [idx, p] : par) {
            const auto m = moi.at(idx);

            sig[idx] = { idx, p, m };
        }

        return sig;
    }

}

