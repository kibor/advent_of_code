#pragma once

#include <charconv>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <ranges>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "common.h"

namespace task2 {

bool possible_set(const std::string_view color, int count) {
    const int RED_LIMIT = 12;
    const int GREEN_LIMIT = 13;
    const int BLUE_LIMIT = 14;
    
    if (color == "red") {
        if (count > RED_LIMIT) {
            return false;
        }
    }
    else if (color == "green") {
        if (count > GREEN_LIMIT) {
            return false;
        }
    }
    else if (color == "blue") {
        if (count > BLUE_LIMIT) {
            return false;
        }
    }
    else {
        std::cerr << "Unknown color " << color << std::endl;
        exit(1);
    }

    return true;
}

bool game_is_possible(const std::string_view game_str) {
    for (auto game_set_str : common::split_string(game_str, "; ")) {
        for (auto result_str : common::split_string(game_set_str, ", ")) {
            auto result_pair_str = common::split_string(result_str, " ");
            
            if (result_pair_str.size() != 2) {
                std::cerr << "Cannot parse game result" << std::endl;
                exit(1);
            }

            int count = common::parse_int(result_pair_str[0]);
            auto color = result_pair_str[1];
            if (!possible_set(color, count)) {
                return false;
            }
        }
    }

    return true;
}

int game_power(const std::string_view game_str) {
    std::unordered_map<std::string_view, int> counts;

    for (auto game_set_str : common::split_string(game_str, "; ")) {
        for (auto result_str : common::split_string(game_set_str, ", ")) {
            auto result_pair_str = common::split_string(result_str, " ");

            if (result_pair_str.size() != 2) {
                std::cerr << "Cannot parse game result" << std::endl;
                exit(1);
            }

            int count = common::parse_int(result_pair_str[0]);
            auto color = result_pair_str[1];
            if (counts[color] < count) {
                counts[color] = count;
            }
        }
    }

    int result = 1;
    for (const auto& [key, value] : counts) {
        result *= value;
    }

    return result;
}

int main()
{
    std::ifstream input("2.input");

    if (!input) {
        std::cerr << "Can't open file.";
        return 1;
    }

    std::string line;
    long result = 0;
    long game_number = 0;
    while (std::getline(input, line))
    {
        ++game_number;
        std::cout << "New game " << game_number << ": " << line << std::endl;
        auto parsed_line = common::split_string(line, ": ");
        if (parsed_line.size() != 2) {
            std::cerr << "Cannot parse line correctly." << std::endl;
            exit(1);
        }

        result += game_power(parsed_line[1]);
    }

    std::cout << "Result is " << result << std::endl;
    return 0;
}

} // task2