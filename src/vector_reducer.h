#pragma once

#include <iostream>
#include <vector>
#include <unordered_set>
#include <random>

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
    std::string strategy;
};

class VectorReducer {
    int dimension;
    std::vector<Vector> targets;
    std::vector<Vector> basis;

    std::uniform_real_distribution<double> uniform;
    std::unordered_set<Vector> uncovered;
    std::unordered_set<Vector> pool;
    std::vector<Vector> vectors;
    std::vector<Candidate> steps;
public:
    VectorReducer();

    void setTargets(const std::vector<std::vector<int>>& newTargets);

    void printTask() const;
    int reduce(const ReduceParameters& parameters, std::mt19937& generator);
private:
    void initialize();
    void addCandidate(const Candidate& candidate);
    void removeUnused();

    bool isCovered(const Vector& target) const;
    bool verify() const;

    double getScore(const Vector& vector, const ReduceParameters& parameters) const;

    std::vector<Candidate> getCandidates(int maxAbsValue) const;
    std::vector<double> scoreCandidates(const std::vector<Candidate>& candidates, const ReduceParameters& parameters) const;

    size_t selectCandidate(const std::vector<double> &scores, const std::string& strategy, std::mt19937& generator);
    size_t selectGreedyCandidate(const std::vector<double> &scores);
    size_t selectGreedyAlternativeCandidate(const std::vector<double> &scores, std::mt19937& generator);
    size_t selectGreedyRandomCandidate(const std::vector<double> &scores, std::mt19937& generator);
};
