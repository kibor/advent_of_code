#include <fstream>
#include <string>
#include <iostream>
#include <string_view>
#include <algorithm>
#include <stdio.h>

#include "common.h"
#include "8.h"

namespace {


} // namespace

namespace task8 {

int main() {
    std::ifstream input("8.input");
    VERIFY(input, << "Can't open file");

    std::string directions;
    VERIFY(std::getline(input, directions), << "Can't read directions");

    std::cout << "Directions are " << directions << std::endl;

    std::string line;
    std::getline(input, line);
    VERIFY(line.empty(), << "Must be empty");

    while (std::getline(input, line)) {
        constexpr size_t NAME_LENGTH = 4;
        char src[NAME_LENGTH] = {};
        char left[NAME_LENGTH] = {};
        char right[NAME_LENGTH] = {};

        sscanf(line.c_str(), "%3s = (%3s, %3s)", src, left, right);

        std::cout << "Src = " << src << ", left = " << left << ", right = " << right << std::endl;

    }

    unsigned long result = 0;
    
    std::cout << "Result is " << result << std::endl;
    return 0;
}


} // task8