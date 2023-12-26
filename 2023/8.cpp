#include <fstream>
#include <string>
#include <iostream>
#include <string_view>
#include <algorithm>
#include <vector>
#include <stdio.h>
#include <unordered_map>

#include "common.h"
#include "8.h"

namespace {

    typedef std::unordered_map<std::string, std::pair<std::string, std::string>> DesertMap;

    unsigned long solve_part_1(const std::string& directions, DesertMap& desert_map) {
        unsigned long result = 0;
        std::string current = "AAA";
        
        while (true) {
            std::cout << "Current place = " << current << std::endl;
            if (current == "ZZZ") {
                break;
            }

            char direction = directions[result % directions.size()];
            VERIFY(direction == 'L' || direction == 'R', << "Wrong direction");
            VERIFY(desert_map.contains(current), << "Unknown direction");

            const auto& value = desert_map[current];
            current = direction == 'L' ? value.first : value.second;

            ++result;
        }

        return result;
    }


    typedef std::vector<std::string_view> Points;

    unsigned long count_matches(const Points& points) {
        unsigned long result = 0;
        for (const auto& p : points) {
            if (p.ends_with('Z')) {
                ++result;
            }
        }

        return result;
    }

    unsigned long solve_part_2(const std::string& directions, DesertMap& desert_map) {
        unsigned long result = 0;
        Points points;

        for (const auto& [key, value] : desert_map) {
            if (key.ends_with('A')) {
                points.emplace_back(key);
                std::cout << "Starting point " << key << std::endl;
            }
        }


        for (const auto& p : points) {
            std::string_view starting_point = p;
            int steps = 0;

            std::cout << "Checking starting point " << p << std::endl;
            while (true) {
                if (starting_point.ends_with('Z')) {
                    break;
                }

                char direction = directions[steps % directions.size()];
                VERIFY(direction == 'L' || direction == 'R', << "Wrong direction");
                const auto& value = desert_map[std::string(starting_point)];
                starting_point = direction == 'L' ? value.first : value.second;

                ++steps;
            }

            std::cout << "For starting point " << p << " number of steps is " << steps << std::endl;
        }

        return result;
    }


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

    DesertMap desert_map;
    while (std::getline(input, line)) {
        constexpr size_t NAME_LENGTH = 4;
        char src[NAME_LENGTH] = {};
        char left[NAME_LENGTH] = {};
        char right[NAME_LENGTH] = {};

        sscanf(line.c_str(), "%3s = (%3s, %3s)", src, left, right);
        VERIFY(!desert_map.contains(src), << "Found duplicate");

        desert_map.insert(std::make_pair(src, std::make_pair(left, right)));
    }

    for (const auto& [key, value] : desert_map) {
        std::cout << "src = " << key << ", left = " << value.first << ", right = " << value.second << std::endl;
    }

    // unsigned long result = solve_part_1(directions, desert_map);
    unsigned long result = solve_part_2(directions, desert_map);
    std::cout << "Result is " << result << std::endl;
    return 0;
}


} // task8