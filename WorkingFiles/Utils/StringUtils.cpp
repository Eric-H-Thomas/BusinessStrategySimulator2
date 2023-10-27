//
// Created by Eric Thomas on 9/20/23.
//

#include "StringUtils.h"
#include <algorithm>

// TODO: test this method
std::string StringUtils::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

// TODO: test this method
bool StringUtils::equalsIgnoreCase(const std::string& str1, const std::string& str2) {
    if (str1.size() != str2.size()) {
        return false;
    }
    for (size_t i = 0; i < str1.size(); ++i) {
        if (std::tolower(str1[i]) != std::tolower(str2[i])) {
            return false;
        }
    }
    return true;
}

// TODO: test this method
bool StringUtils::equalsIgnoreCaseAndIgnoreUnderscores(const std::string& str1, const std::string& str2) {
    // Remove underscores from both strings and then compare them case-insensitively
    std::string str1NoUnderscores = str1;
    str1NoUnderscores.erase(std::remove(str1NoUnderscores.begin(), str1NoUnderscores.end(), '_'), str1NoUnderscores.end());

    std::string str2NoUnderscores = str2;
    str2NoUnderscores.erase(std::remove(str2NoUnderscores.begin(), str2NoUnderscores.end(), '_'), str2NoUnderscores.end());

    return equalsIgnoreCase(str1NoUnderscores, str2NoUnderscores);
}
