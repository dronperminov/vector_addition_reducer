#include "vector.h"

Vector::Vector(int dimension) : values(dimension, 0) {
    this->dimension = dimension;
}

Vector::Vector(int dimension, int basis) : values(dimension, 0) {
    this->dimension = dimension;
    this->values[basis] = 1;
}

Vector::Vector(const std::vector<int>& values) {
    this->dimension = (int) values.size();
    this->values = values;
}

Vector Vector::operator+(const Vector& vector) const {
    Vector result(dimension);

    for (int i = 0; i < dimension; i++)
        result.values[i] = values[i] + vector.values[i];

    return result;
}

Vector Vector::operator-(const Vector& vector) const {
    Vector result(dimension);

    for (int i = 0; i < dimension; i++)
        result.values[i] = values[i] - vector.values[i];

    return result;
}

Vector Vector::operator-() const {
    Vector result(dimension);

    for (int i = 0; i < dimension; i++)
        result.values[i] = -values[i];

    return result;
}

bool Vector::operator==(const Vector& vector) const {
    if (dimension != vector.dimension)
        return false;

    for (int i = 0; i < dimension; i++)
        if (values[i] != vector.values[i])
            return false;

    return true;
}

bool Vector::operator!=(const Vector& vector) const {
    return !(*this == vector);
}

int Vector::compare(const Vector& vector) const {
    if (dimension != vector.dimension)
        return 0;

    bool equal = true;
    bool inverse = true;

    for (int i = 0; i < dimension && (equal || inverse); i++) {
        if (values[i] != vector.values[i])
            equal = false;

        if (values[i] != -vector.values[i])
            inverse = false;
    }

    if (equal)
        return 1;

    return inverse ? -1 : 0;
}

int Vector::hammingDistance(const Vector& vector) const {
    int distance = 0;
    int distanceInverse = 0;

    for (int i = 0; i < dimension; i++) {
        if (values[i] != vector.values[i])
            distance++;

        if (values[i] != -vector.values[i])
            distanceInverse++;
    }

    return std::min(distance, distanceInverse);
}

int Vector::matchesCount(const Vector& vector) const {
    int matches = 0;

    for (int i = 0; i < dimension; i++)
        if (values[i] != 0 && values[i] == vector.values[i])
            matches++;

    return matches;
}

int Vector::maxAbs() const {
    int max = 0;

    for (int value : values)
        max = std::max(std::abs(value), max);

    return max;
}

std::string Vector::toKey() const {
    std::stringstream ss;

    for (int i = 0; i < dimension; i++)
        ss << values[i];

    return ss.str();
}

std::ostream& operator<<(std::ostream& os, const Vector& vector) {
    os << "[";

    for (int i = 0; i < vector.dimension; i++)
        os << (i > 0 ? ", " : "") << std::setw(2) << vector.values[i];

    return os << "]";
}
