#pragma once

#include <ctime>
#include <random>

namespace Random {
    inline std::mt19937& engine() {
        static std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
        return rng;
    }

    inline int nextInt(int upperExclusive) {
        if (upperExclusive <= 0) {
            return 0;
        }
        std::uniform_int_distribution<int> dist(0, upperExclusive - 1);
        return dist(engine());
    }
}