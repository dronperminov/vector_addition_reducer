#include "vector_reducer.h"

VectorReducer::VectorReducer() {
    dimension = 0;
}

void VectorReducer::setTargets(const std::vector<std::vector<int>>& newTargets) {
    dimension = (int) newTargets[0].size();

    targets.clear();
    basis.clear();

    for (const auto& target : newTargets)
        targets.emplace_back(Vector(target));

    for (int i = 0; i < dimension; i++)
        basis.emplace_back(Vector(dimension, i));
}

void VectorReducer::printTask() const {
    std::cout << "- dimension: " << dimension << std::endl;
    std::cout << "- targets (" << targets.size() << "):" << std::endl;

    for (const Vector& target : targets)
        std::cout << "  " << target << std::endl;
}

int VectorReducer::reduce(const ReduceParameters& parameters) {
    initialize();

    if (parameters.verbose)
        std::cout << "Start reducing. Uncovered: " << uncovered.size() << " / " << targets.size() << std::endl;

    while (!uncovered.empty()) {
        std::vector<Candidate> candidates = getCandidates(parameters.maxAbsValue);
        std::vector<double> scores;

        for (const Candidate& candidate : candidates)
            scores.push_back(getScore(candidate.vector, parameters));

        size_t imax = 0;
        for (size_t i = 1; i < scores.size(); i++)
            if (scores[i] > scores[imax])
                imax = i;

        addCandidate(candidates[imax]);

        if (parameters.verbose) {
            std::cout << steps.size() << ": " << candidates.size() << " candidates, best score: " << scores[imax];
            std::cout << " (v" << (dimension + steps.size() - 1) << " = v" << candidates[imax].i << (candidates[imax].sign > 0 ? " + " : " - ") << "v" << candidates[imax].j << "): " << candidates[imax].vector;
            std::cout << ", lost: " << uncovered.size() << std::endl;
        }
    }

    if (parameters.verbose)
        std::cout << "Additions before reducing: " << steps.size() << std::endl;

    removeUnused();

    if (parameters.verbose) {
        std::cout << "Additions after reducing: " << steps.size() << std::endl;
        std::cout << "Verification: " << (verify() ? "ok" : "fail") << std::endl;
    }

    return steps.size();
}

void VectorReducer::initialize() {
    uncovered.clear();
    pool.clear();
    vectors.clear();
    steps.clear();

    for (const Vector& vector : basis) {
        vectors.push_back(vector);
        pool.insert(vector);
        pool.insert(-vector);
    }

    for (const Vector& target : targets)
        if (!isCovered(target))
            uncovered.insert(target);
}

void VectorReducer::addCandidate(const Candidate& candidate) {
    pool.insert(candidate.vector);
    pool.insert(-candidate.vector);

    vectors.push_back(candidate.vector);
    steps.push_back(candidate);

    for (auto it = uncovered.begin(); it != uncovered.end(); ) {
        if (isCovered(*it))
            it = uncovered.erase(it);
        else
            it++;
    }
}

void VectorReducer::removeUnused() {
    std::unordered_set<Vector> targetSet;
    for (const Vector& target : targets) {
        targetSet.insert(target);
        targetSet.insert(-target);
    }

    std::vector<bool> used(vectors.size(), false);
    for (int i = steps.size() - 1; i >= 0; i--) {
        const Candidate& step = steps[i];

        if (used[dimension + i]) {
            used[step.i] = true;
            used[step.j] = true;
            continue;
        }

        if (targetSet.find(step.vector) != targetSet.end()) {
            used[dimension + i] = true;
            used[step.i] = true;
            used[step.j] = true;
        }
    }

    std::vector<int> indices(vectors.size());
    for (int i = 0; i < dimension; i++)
        indices[i] = i;

    size_t j = 0;
    for (size_t i = 0; i < steps.size(); i++) {
        if (!used[dimension + i])
            continue;

        indices[dimension + i] = dimension + j;
        steps[i].i = indices[steps[i].i];
        steps[i].j = indices[steps[i].j];

        if (j != i) {
            steps[j] = steps[i];
            vectors[dimension + j] = vectors[dimension + i];
        }

        j++;
    }

    steps.erase(steps.begin() + j, steps.end());
    vectors.erase(vectors.begin() + j + dimension, vectors.end());
}

bool VectorReducer::verify() const {
    std::unordered_set<Vector> targetSet;
    for (const Vector& target : targets)
        targetSet.insert(target);

    for (const Vector& vector : basis)
        targetSet.erase(vector);

    for (size_t i = 0; i < steps.size(); i++) {
        const Candidate& step = steps[i];

        if (step.i < 0 || step.i >= dimension + i || step.j < 0 || step.j >= dimension + i)
            return false;

        Vector vector = step.sign > 0 ? (vectors[step.i] + vectors[step.j]) : (vectors[step.i] - vectors[step.j]);
        if (step.vector != vector || vectors[dimension + i] != vector)
            return false;

        Vector inverse = -vector;

        if (targetSet.find(vector) != targetSet.end()) {
            targetSet.erase(vector);
        }
        else if (targetSet.find(inverse) != targetSet.end()) {
            targetSet.erase(inverse);
        }
    }

    return targetSet.empty();
}

bool VectorReducer::isCovered(const Vector& target) const {
    return pool.find(target) != pool.end();
}

std::vector<Candidate> VectorReducer::getCandidates(int maxAbsValue) const {
    std::vector<Candidate> candidates;

    for (size_t i = 0; i < vectors.size(); i++) {
        for (size_t j = i + 1; j < vectors.size(); j++) {
            const Vector& vi = vectors[i];
            const Vector& vj = vectors[j];
            std::vector<Candidate> vs = {
                {i, j, 1, vi + vj}, 
                {i, j, -1, vi - vj}
            };

            for (const Candidate& candidate: vs) {
                if (pool.find(candidate.vector) != pool.end())
                    continue;

                if (maxAbsValue > 0 && candidate.vector.maxAbs() > maxAbsValue)
                    continue;

                candidates.emplace_back(candidate);
            }
        }
    }

    return candidates;
}

double VectorReducer::getScore(const Vector& vector, const ReduceParameters& parameters) {
    double score = 0.0;

    for (const Vector& target : uncovered) {
        if (vector.compare(target)) {
            score += parameters.coverWeight;
            continue;
        }

        if (parameters.oneStepWeight > 0) {
            bool foundOneStep = false;
            for (const Vector& v : vectors) {
                if (target.compare(vector + v) || target.compare(vector - v)) {
                    foundOneStep = true;
                    break;
                }
            }

            if (foundOneStep)
                score += parameters.oneStepWeight;
        }

        int hamming = vector.hammingDistance(target);
        int matches = vector.matchesCount(target);

        if (hamming < dimension)
            score += (dimension - hamming) * parameters.hammingWeight;

        score += matches * parameters.matchesWeight;
    }

    return score;
}
