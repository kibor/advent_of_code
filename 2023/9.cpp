#include <fstream>
#include <string>
#include <iostream>
#include <string_view>
#include <algorithm>
#include <vector>
#include <list>
#include <stdio.h>
#include <unordered_map>

#include "common.h"
#include "9.h"

namespace {

typedef std::vector<long> Sequence;
typedef std::list<Sequence> SequenceList;

Sequence extract_sequence(const std::string& line, bool solve_second_half) {
    Sequence sequence;
    for (const auto& word : common::split_string(line, " ")) {
        const auto number = common::parse_number<long>(word);
        sequence.push_back(number);

        std::cout << number << " ";
    }

    std::cout << std::endl;

    if (solve_second_half) {
        std::reverse(sequence.begin(), sequence.end());
    }
    return sequence;
}

bool all_zeros(const Sequence& sequence) {
    return std::ranges::find_if(sequence, [](long number) { return number != 0; }) == sequence.end();
}

SequenceList populate_sequence(const Sequence& sequence) {
    SequenceList result;
    result.push_back(sequence);

    while (true) {
        const auto& last_sequence = result.back();
        VERIFY(last_sequence.size() > 1, << "Sequence is too short");

        if (all_zeros(last_sequence)) {
            break;
        }

        auto& new_sequence = result.emplace_back();
        for (int i = 0; i < last_sequence.size() - 1; ++i) {
            new_sequence.push_back(last_sequence[i + 1] - last_sequence[i]);
        }
    }

    return result;
}

long calculate_last_element(const SequenceList& sequence_list) {
    long next_element = 0;
    for (const auto& sequence : std::views::reverse(sequence_list)) {
        VERIFY(!sequence.empty(), << "Empty sequence");

        next_element += sequence.back();
    }

    std::cout << "Next element is " << next_element << std::endl;
    return next_element;
}

long calculate_first_element(const SequenceList& sequence_list) {
    long first_element = 0;
    for (const auto& sequence : std::views::reverse(sequence_list)) {
        VERIFY(!sequence.empty(), << "Empty sequence");

        first_element = sequence.front() - first_element;
    }

    std::cout << "First element is " << first_element << std::endl;
    return first_element;
}

long get_last_number(const std::string& line, bool solve_second_half) {
    const auto sequence = extract_sequence(line, solve_second_half);
    const auto sequence_list = populate_sequence(sequence);

    return calculate_last_element(sequence_list);
}

} // namespace

namespace task9 {

int main() {
    std::ifstream input("9.input");
    VERIFY(input, << "Can't open file");
    
    bool solve_second_half = true;

    std::string line;
    long result = 0;
    while (std::getline(input, line)) {
        result += get_last_number(line, solve_second_half);
    }

    std::cout << "Result is " << result << std::endl;
    return 0;
}


} // task9