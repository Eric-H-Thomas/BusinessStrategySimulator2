//
// Created by Eric Thomas on 9/14/23.
//

#include "MiscUtils.h"

#include <iostream>
#include <vector>
#include <random>

int MiscUtils::choose_index_given_probabilities(const std::vector<double>& probabilities) {
    // TODO: need to test this method

    // Check if the probabilities vector is empty
    if (probabilities.empty()) {
        std::cerr << "Error: Probabilities vector is empty." << std::endl;
        return -1; // Return an error code
    }

    // Calculate the cumulative probabilities
    std::vector<double> cumulativeProbabilities(probabilities.size());
    cumulativeProbabilities[0] = probabilities[0];
    for (size_t i = 1; i < probabilities.size(); ++i) {
        cumulativeProbabilities[i] = cumulativeProbabilities[i - 1] + probabilities[i];
    }

    // Generate a random number between 0 and 1
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> distribution(0.0, 1.0);
    double randomValue = distribution(gen);

    // Find the index corresponding to the random value
    for (size_t i = 0; i < cumulativeProbabilities.size(); ++i) {
        if (randomValue < cumulativeProbabilities[i]) {
            return static_cast<int>(i); // Return the chosen index
        }
    }

    // In case something goes wrong (shouldn't happen)
    std::cerr << "Error: Unable to choose an index." << std::endl;
    throw std::exception();
}


// Dot product of two vectors of doubles
double MiscUtils::dot_product(const std::vector<double>& vector1, const std::vector<double>& vector2) {
    if (vector1.size() != vector2.size()) {
        // Ensure that both vectors have the same dimension.
        throw std::invalid_argument("Vectors must have the same dimension");
    }

    double result = 0.0;

    for (size_t i = 0; i < vector1.size(); ++i) {
        result += vector1[i] * vector2[i];
    }

    return result;
}

// Dot product of two vectors of ints
int MiscUtils::dot_product(const std::vector<int>& vector1, const std::vector<int>& vector2) {
    if (vector1.size() != vector2.size()) {
        // Ensure that both vectors have the same dimension.
        throw std::invalid_argument("Vectors must have the same dimension");
    }

    int result = 0;

    for (size_t i = 0; i < vector1.size(); ++i) {
        result += vector1[i] * vector2[i];
    }

    return result;
}

// Dot product of a vector of ints with a vector of doubles
double MiscUtils::dot_product(const std::vector<int>& vector1, const std::vector<double>& vector2) {
    if (vector1.size() != vector2.size()) {
        // Ensure that both vectors have the same dimension.
        throw std::invalid_argument("Vectors must have the same dimension");
    }

    double result = 0.0;

    for (size_t i = 0; i < vector1.size(); ++i) {
        result += vector1[i] * vector2[i];
    }

    return result;
}

// Dot product of a vector of doubles with a vector of ints
double MiscUtils::dot_product(const std::vector<double>& vector1, const std::vector<int>& vector2) {
    if (vector1.size() != vector2.size()) {
        // Ensure that both vectors have the same dimension.
        throw std::invalid_argument("Vectors must have the same dimension");
    }

    double result = 0.0;

    for (size_t i = 0; i < vector1.size(); ++i) {
        result += vector1[i] * vector2[i];
    }

    return result;
}

template <typename T> T MiscUtils::choose_random_from_set(const std::set<T>& inputSet) {
    // Convert the set to a vector
    std::vector<T> myVector(inputSet.begin(), inputSet.end());

    // Seed the random number generator
    std::random_device rd;
    std::mt19937 gen(rd());

    // Generate a random index within the bounds of the vector
    std::uniform_int_distribution<> dis(0, myVector.size() - 1);
    int randomIndex = dis(gen);

    // Return the randomly chosen element
    return myVector[randomIndex];
}

double MiscUtils::get_percentage_overlap(const std::vector<int>& vector1, const std::vector<int>& vector2) {
    // Check if the vectors have the same size
    if (vector1.size() != vector2.size()) {
        throw std::invalid_argument("Input vectors must have the same size.");
    }

    // Initialize a counter for the overlap
    int overlapCount = 0;

    // Iterate through the vectors and count the overlap
    for (size_t i = 0; i < vector1.size(); ++i) {
        if (vector1[i] == 1 && vector2[i] == 1) {
            overlapCount++;
        }
    }

    // Calculate the percentage overlap
    double percentageOverlap = (static_cast<double>(overlapCount) / vector1.size());

    return percentageOverlap;
}