#pragma once

#include <fstream>
#include <string>
#include <iostream>
#include <string_view>
#include <ranges>
#include <tuple>

#include "common.h"

namespace task6 {

std::vector<unsigned long> extract_numbers(const std::string& line) {
    std::vector<unsigned long> result;
    auto lines = common::split_string(line, " ");
    
    // Skip first word
    lines.erase(lines.begin());
    for (const auto number : lines 
            | std::views::transform(common::trim)
            | std::views::filter([](const std::string_view& str) { return !str.empty(); })
            | std::views::transform(common::parse_number<unsigned long>)
        ) {
            std::cout << "Found number " << number << std::endl;
            result.push_back(number);
        }
        
    return result;
}

int get_win_count(unsigned long time, unsigned long record) {
    int win_count = 0;

    for (unsigned long seconds = 1; seconds < time; ++seconds) {
        unsigned long speed = seconds;
        unsigned long time_left = time - seconds;
        unsigned long distance = time_left * speed;

        if (distance > record) {
            ++win_count;
        }
    }

    std::cout << "Win count for time " << time << " and record " << record << " is " << win_count << std::endl; 

    return win_count;
}

unsigned long main() {
    std::ifstream input("6.input");
    VERIFY(input, << "Can't open file");

    std::string line;
    VERIFY(std::getline(input, line), << "Can't read line");
    auto times = extract_numbers(line);

    VERIFY(std::getline(input, line), << "Can't read line");
    auto records = extract_numbers(line);

    VERIFY(times.size() == records.size(), << "Sizes do not match");

    int result = 1;
    for (int i = 0; i < times.size(); ++i) {
        result *= get_win_count(times[i], records[i]);
    }

    std::cout << "Result is " << result << std::endl;
    return 0;
}

} // task6