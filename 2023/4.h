#pragma once

#include <algorithm>
#include <fstream>
#include <string>
#include <iostream>
#include <string_view>
#include <unordered_set>
#include <unordered_map>

#include "common.h"

namespace task4 {

int get_winning_cards_count(const std::string& line) {
    auto it = std::ranges::find(line, ':');
    VERIFY(it != line.end(), << "Can't find delimeter");

    std::string_view numbers(it + 1, line.end());
    std::unordered_set<int> winning_numbers;
    bool read_all_winning_cards = false;
    int won_count = 0;

    for (const auto word : common::split_string(numbers, " ") 
            | std::views::transform(common::trim)
            | std::views::filter([](const std::string_view& str) { return !str.empty(); })
        ) {

        if (word == "|") {
            read_all_winning_cards = true;
            continue;
        }

        int number = common::parse_number<int>(word);
        if (read_all_winning_cards) {
            won_count += winning_numbers.contains(number) ? 1 : 0;
        } else {
            winning_numbers.insert(number);
        }
    }

    return won_count;
}

int get_card_score(const std::string& line) {
    int cards_count = get_winning_cards_count(line);
    return cards_count == 0 ? 0 : 1 << (cards_count - 1);
}


std::unordered_map<int, int> cards_copies;

void update_cards_copies_count(int card_id, const std::string& line) {
    int cards_count = get_winning_cards_count(line);
    ++cards_copies[card_id];

    for (int i = card_id + 1; i <= card_id + cards_count; ++i) {
        cards_copies[i] += cards_copies[card_id];
    }
}

int get_cards_copies_count() {
    int result = 0;

    for (const auto& [key, value] : cards_copies) {
        std::cout << "Card " << key << " has count of " << value << std::endl;
        result += value;
    }

    return result;
}

int main()
{
    std::ifstream input("4.input");
    VERIFY(input, << "Can't open file.");

    std::string line;
    long result = 0;
    long card_id = 0;
    while (std::getline(input, line))
    {
        ++card_id;
        int score = get_card_score(line);
        result += score;

        update_cards_copies_count(card_id, line);
    }

    std::cout << "Result is " << result << std::endl;

    int copies_count = get_cards_copies_count();
    std::cout << "Copies count is " << copies_count << std::endl;

    return 0;
}

} // task4