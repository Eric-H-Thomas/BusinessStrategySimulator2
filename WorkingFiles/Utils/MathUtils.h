//
// Created by Eric Thomas on 9/14/23.
//

#pragma once
#include <iostream>
#include <vector>

class MathUtils {
public:
    static int choose_index_given_probabilities(const std::vector<double>& probabilities);
    static double dot_product(const std::vector<double>& vector1, const std::vector<double>& vector2);
    static int dot_product(const std::vector<int>& vector1, const std::vector<int>& vector2);
    static double dot_product(const std::vector<int>& vector1, const std::vector<double>& vector2);
    static double dot_product(const std::vector<double>& vector1, const std::vector<int>& vector2);
};
