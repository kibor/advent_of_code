#pragma once

#include <fstream>
#include <string>
#include <iostream>
#include <iterator>
#include <string_view>
#include <optional>
#include <ranges>
#include <unordered_set>

#include "common.h"

namespace task4 {

std::string trim(const std::string_view sv) {
    std::string s(sv);

    auto view = s 
        | std::views::drop_while(isspace) 
        | std::views::reverse 
        | std::views::drop_while(isspace)
        | std::views::reverse;

    std::string result{view.begin(), view.end()};
    return result;
}

int get_card_score(const std::string& line) {
    auto it = std::ranges::find(line, ':');
    if (it == line.end()) {
        std::cerr << "Can't find delimeter" << std::endl;
        exit(1);
    }

    std::string_view numbers(it + 1, line.end());
    std::unordered_set<int> winning_numbers;
    bool read_all_winning_cards = false;
    int won_count = 0;

    for (const auto token : common::split_string(numbers, " ")) {
        auto word = trim(token);
        if (word.empty()) {
            continue;
        }

        std::cout << "Processing word " << word << std::endl;
        if (word == "|") {
            read_all_winning_cards = true;
            continue;
        }

        int number = common::parse_int(word);
        if (read_all_winning_cards) {
            won_count += winning_numbers.contains(number) ? 1 : 0;
        } else {
            winning_numbers.insert(number);
        }
    }

    return won_count == 0 ? 0 : 1 << (won_count - 1);
}

int main()
{
    std::ifstream input("4.input");

    if (!input) {
        std::cerr << "Can't open file.";
        return 1;
    }

    std::string line;
    long result = 0;
    while (std::getline(input, line))
    {
        int number = get_card_score(line);
        result += number;
    }

    std::cout << "Result is " << result << std::endl;
    return 0;
}

} // task4