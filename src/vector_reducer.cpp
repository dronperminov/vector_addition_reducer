#include "vector_reducer.h"

VectorReducer::VectorReducer(const std::vector<std::vector<int>>& targets) {
    this->dimension = (int) targets[0].size();

    for (const auto& target : targets)
        this->targets.emplace_back(Vector(target));

    for (int i = 0; i < dimension; i++)
        this->basis.emplace_back(Vector(dimension, i));
}

void VectorReducer::printTask() const {
    std::cout << "- dimension: " << dimension << std::endl;
    std::cout << "- targets (" << targets.size() << "):" << std::endl;

    for (const Vector& target : targets)
        std::cout << "  " << target << std::endl;
}

int VectorReducer::reduce(const ReduceParameters& parameters) {
    uncovered.clear();
    pool.clear();
    vectors.clear();
    steps.clear();

    for (const Vector& vector : basis) {
        vectors.push_back(vector);
        pool.insert(vector);
    }

    for (const Vector& target : targets)
        if (!isCovered(target))
            uncovered.insert(target);

    std::cout << "Uncovered: " << uncovered.size() << " / " << targets.size() << std::endl;

    for (int step = 0; !uncovered.empty(); step++) {
        std::vector<Candidate> candidates = getCandidates(parameters.maxAbsValue);
        std::vector<double> scores;

        for (const Candidate& candidate : candidates)
            scores.push_back(getScore(candidate.vector, parameters));

        size_t imax = 0;
        for (size_t i = 1; i < scores.size(); i++)
            if (scores[i] > scores[imax])
                imax = i;

        const Candidate& candidate = candidates[imax];

        pool.insert(candidates[imax].vector);
        vectors.push_back(candidate.vector);
        steps.push_back(candidate);

        for (auto it = uncovered.begin(); it != uncovered.end(); ) {
            if (isCovered(*it))
                it = uncovered.erase(it);
            else
                it++;
        }

        std::cout << (step + 1) << ": " << candidates.size() << " candidates, best score: " << scores[imax] << " (v" << candidate.i << (candidate.sign > 0 ? " + " : " - ") << "v" << candidate.j << "): " << candidate.vector << ", lost: " << uncovered.size() << std::endl;
    }

    // TODO: remove unused
    return steps.size();
}

bool VectorReducer::isCovered(const Vector& target) const {
    for (const Vector& vector : pool)
        if (vector.compare(target))
            return true;

    return false;
}

std::vector<Candidate> VectorReducer::getCandidates(int maxAbsValue) const {
    std::vector<Candidate> candidates;

    for (size_t i = 0; i < vectors.size(); i++) {
        for (size_t j = i + 1; j < vectors.size(); j++) {
            const Vector& vi = vectors[i];
            const Vector& vj = vectors[j];
            std::vector<Candidate> vs = {
                {i, j, 1, vi + vj}, 
                {i, j, -1, vi - vj},
                {j, i, -1, vj - vi}
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
