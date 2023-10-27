//
// Created by Eric Thomas on 9/20/23.
//

#pragma once
#include <string>

class StringUtils {
public:
    static std::string toUpper(const std::string& str);
    static bool equalsIgnoreCase(const std::string& str1, const std::string& str2);
    static bool equalsIgnoreCaseAndIgnoreUnderscores(const std::string& str1, const std::string& str2);
};

