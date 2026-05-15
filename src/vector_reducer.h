#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "entities/vector.h"

struct Candidate {
    size_t i;
    size_t j;
    int sign;
    Vector vector;
};

struct ReduceParameters {
    bool verbose;
    int maxAbsValue;
    double coverWeight;
    double oneStepWeight;
    double hammingWeight;
    double matchesWeight;
};

class VectorReducer {
    int dimension;
    std::vector<Vector> targets;
    std::vector<Vector> basis;

    std::unordered_set<Vector> uncovered;
    std::unordered_set<Vector> pool;
    std::vector<Vector> vectors;
    std::vector<Candidate> steps;
public:
    VectorReducer();

    void setTargets(const std::vector<std::vector<int>>& newTargets);

    void printTask() const;
    int reduce(const ReduceParameters& parameters);
private:
    void initialize();
    void addCandidate(const Candidate& candidate);
    void removeUnused();

    bool isCovered(const Vector& target) const;
    bool verify() const;

    std::vector<Candidate> getCandidates(int maxAbsValue) const;
    double getScore(const Vector& vector, const ReduceParameters& parameters);
};
