#pragma once

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

class Vector {
    int dimension;
    std::vector<int> values;
public:
    Vector(int dimension);
    Vector(int dimension, int basis);
    Vector(const std::vector<int>& values);

    Vector operator+(const Vector& vector) const;
    Vector operator-(const Vector& vector) const;
    Vector operator-() const;

    bool operator==(const Vector& vector) const;
    bool operator!=(const Vector& vector) const;
    int compare(const Vector& vector) const;
    int hammingDistance(const Vector& vector) const;
    int matchesCount(const Vector& vector) const;

    int maxAbs() const;

    std::string toKey() const;

    friend std::ostream& operator<<(std::ostream& os, const Vector& vector);
    friend struct std::hash<Vector>;
};

namespace std {
    template <>
    struct hash<Vector> {
        size_t operator()(const Vector& vector) const {
            size_t seed = 0;

            for (int value : vector.values)
                seed ^= std::hash<int>{}(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

            return seed;
        }
    };
}
