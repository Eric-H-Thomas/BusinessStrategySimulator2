//
// Created by Eric Thomas on 9/14/23.
//

#pragma once
#include <iostream>
#include <vector>

class MathUtils {
public:
    static double dotProduct(const std::vector<double>& vector1, const std::vector<double>& vector2);
    static int dotProduct(const std::vector<int>& vector1, const std::vector<int>& vector2);
    static double dotProduct(const std::vector<int>& vector1, const std::vector<double>& vector2);
    static double dotProduct(const std::vector<double>& vector1, const std::vector<int>& vector2);
};
