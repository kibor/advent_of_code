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

std::string_view trim(const std::string_view sv) {
    auto view = sv
        | std::views::drop_while(isspace) 
        | std::views::reverse 
        | std::views::drop_while(isspace)
        | std::views::reverse;

    return std::string_view{view.begin().base().base(), view.end().base().base()};
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

    for (const auto word : common::split_string(numbers, " ") 
            | std::views::transform(trim)
            | std::views::filter([](const std::string_view& str) { return !str.empty(); })
        ) {

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
    long card_id = 0;
    while (std::getline(input, line))
    {
        ++card_id;
        int score = get_card_score(line);
        result += score;

        std::cout << "Card " << card_id << " has score " << score << std::endl;
    }

    std::cout << "Result is " << result << std::endl;
    return 0;
}

} // task4