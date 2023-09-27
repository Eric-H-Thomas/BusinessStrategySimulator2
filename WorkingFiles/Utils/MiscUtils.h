//
// Created by Eric Thomas on 9/14/23.
//

#pragma once
#include <iostream>
#include <vector>
#include <set>
#include <random>
#include "../Market/Market.h"
using std::set;

class MiscUtils {
public:
    static int choose_index_given_probabilities(const std::vector<double>& probabilities);
    static double dot_product(const std::vector<double>& vector1, const std::vector<double>& vector2);
    static int dot_product(const std::vector<int>& vector1, const std::vector<int>& vector2);
    static double dot_product(const std::vector<int>& vector1, const std::vector<double>& vector2);
    static double dot_product(const std::vector<double>& vector1, const std::vector<int>& vector2);
    template<typename T> static T choose_random_from_set(const std::set<T>& inputSet);
    static double get_percentage_overlap(const std::vector<int>& vector1, const std::vector<int>& vector2);
};
