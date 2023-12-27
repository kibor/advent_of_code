#include <fstream>
#include <string>
#include <iostream>
#include <string_view>
#include <algorithm>
#include <vector>
#include <stdio.h>
#include <unordered_map>

#include "common.h"
#include "9.h"

namespace {

std::vector<long> extract_sequence(const std::string& line) {
    std::vector<long> sequence;
    for (const auto& word : common::split_string(line, " ")) {
        const auto number = common::parse_number<long>(word);
        sequence.push_back(number);

        std::cout << number << " ";
    }

    std::cout << std::endl;
    return sequence;
}

long get_next_number(const std::string& line) {
    const auto sequence = extract_sequence(line);

    return 0;
}

} // namespace

namespace task9 {

int main() {
    std::ifstream input("9.input");
    VERIFY(input, << "Can't open file");

    std::string line;
    long result = 0;
    while (std::getline(input, line)) {
        result += get_next_number(line);
    }

    std::cout << "Result is " << result << std::endl;
    return 0;
}


} // task9