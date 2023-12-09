#pragma once

#include <fstream>
#include <string>
#include <iostream>
#include <iterator>
#include <optional>
#include <ranges>
#include <ctype.h>
#include <unordered_map>
#include <unordered_set>

#include "common.h"

namespace task3 {

struct hash_pair {
    template <class T1, class T2>
    size_t operator()(const std::pair<T1, T2>&p) const
    {
        auto hash1 = std::hash<T1>{}(p.first);
        auto hash2 = std::hash<T2>{}(p.second);

        if (hash1 != hash2) {
            return hash1 ^ hash2;
        }

        // If hash1 == hash2, their XOR is zero.
        return hash1;
    }
};

class Solver {
public:
    Solver(bool accept_all_symbols = true)
    : accept_all_symbols(accept_all_symbols) {

    }

    void add_line(const std::string& line) {
        int curr_y_pos = line_count;
        int curr_x_pos = -1;
        std::optional<NumberDataHolder> curr_number_opt;

        for (char ch : line) {
            ++curr_x_pos;

            if (is_dot(ch)) {
                if (curr_number_opt) {
                    add_number_and_reset(curr_number_opt);
                }

                continue;
            }

            if (is_digit(ch)) {
                if (!curr_number_opt) {
                    curr_number_opt = NumberDataHolder(curr_x_pos, curr_y_pos);
                }

                curr_number_opt->add_digit(ch);
                continue;
            }

            if (is_symbol(ch, accept_all_symbols)) {
                if (curr_number_opt) {
                    add_number_and_reset(curr_number_opt);
                }

                add_symbol(curr_x_pos, curr_y_pos);
                continue;
            }

            VERIFY(!accept_all_symbols, << "We shouldn't be here.");
        }

        if (curr_number_opt) {
            add_number_and_reset(curr_number_opt);
        }

        ++line_count;
    }

    int get_result() const {
        int result = 0;

        std::cout << "Starting to calculate result" << std::endl;
        for (const auto& [key, value] : numbers) {
            auto length = get_number_length(value);

            if (coords_are_adjacent(key.first, key.second, length)) {
                result += value;
            }
        }

        return result;
    }

    size_t get_result2() const {
        size_t result = 0;

        std::cout << "Symbols count is " << symbols.size() << std::endl;

        for (const auto& key : symbols) {
            const auto gears = get_adjacent_numbers(key.first, key.second);
            if (gears.size() < 2) {
                continue;
            }

            VERIFY(gears.size() > 2, << "Too many gears");

            auto it = gears.begin();
            int first_number = *it;
            ++it;
            int second_number = *it;

            std::cout << "New gears are " << first_number << " and " << second_number << std::endl;

            result += first_number * second_number;
        }

        return result;
    }

private:
    struct NumberDataHolder {
        int x;
        int y;
        std::string value;

        NumberDataHolder(int x, int y) {
            this->x = x;
            this->y = y;
        }

        void add_digit(char ch) {
            value += ch;
        }

        int get_value () const {
            return std::stoi(value);
        }
    };

private:
    bool coords_are_adjacent(int x_pos, int y_pos, int length) const {
        for (int x = x_pos - 1; x <= x_pos + length; ++x) {
            if (symbols.contains(std::make_pair(x, y_pos - 1))) {
                return true;
            }

            if (symbols.contains(std::make_pair(x, y_pos + 1))) {
                return true;
            }
        }

        if (symbols.contains(std::make_pair(x_pos - 1, y_pos))) {
            return true;
        }

        if (symbols.contains(std::make_pair(x_pos + length, y_pos))) {
            return true;
        }

        return false;
    }

    std::optional<int> get_number_by_coords(int x, int y) const {
        const int max_length = 3;
        for (int x1 = x - max_length + 1; x1 <= x; ++x1) {
            const auto key = std::make_pair(x1, y);
            const auto value_it = numbers.find(key);

            if (value_it != numbers.end()) {
                auto value = value_it->second;
                int length = get_number_length(value);
                if (x1 + length - 1 >= x) {
                    return std::make_optional(value_it->second);
                }
            }
        }

        return std::nullopt;
    }

    std::unordered_set<int> get_adjacent_numbers(int x, int y) const {
        std::cout << "Checking for symbol with coords " << x << ", " << y << std::endl;
        std::unordered_set<int> v;

        for (int pos_x = x - 1; pos_x <= x + 1; ++pos_x) {
            for (int pos_y = y - 1; pos_y <= y + 1; ++pos_y) {
                auto number_opt = get_number_by_coords(pos_x, pos_y);
                if (number_opt) {
                    std::cout << "Found number " << *number_opt << std::endl;
                    v.insert(*number_opt);
                }
            }
        }

        return v;
    }

    void add_number_and_reset(std::optional<NumberDataHolder>& number) {
        VERIFY(number, << "Optional is not initialized");

        const auto key = std::make_pair(number->x, number->y);
        VERIFY(!numbers.contains(key), << "Duplicate coords, cannot happen.");

        numbers[key] = number->get_value();
        number = std::nullopt;
    }

    void add_symbol(int x, int y) {
        const auto key = std::make_pair(x, y);
        VERIFY(!symbols.contains(key), << "Duplicate coords, cannot happen.");

        symbols.insert(key);
    }


private:
    static bool is_digit(char ch) {
        return isdigit(ch);
    }

    static bool is_dot(char ch) {
        return ch == '.';
    }

    static bool is_symbol(char ch, bool accept_all_symbols) {
        if (accept_all_symbols) {
            return !is_digit(ch) && !is_dot(ch);
        }

        return ch == '*';
    }

    static int get_number_length(int number) {
        int result = 1;
        while (number >= 10) {
            number /= 10;
            ++result;
        }

        return result;
    }

private:
    int line_count = 0;
    const bool accept_all_symbols;

    typedef std::pair<int, int> coords;
    std::unordered_map<coords, int, hash_pair> numbers;
    std::unordered_set<coords, hash_pair> symbols;
};

int main() {
    std::ifstream input("3.input");
    VERIFY(input, << "Can't open file.");

    std::string line;
    Solver solver(false);
    while (std::getline(input, line)) {
        solver.add_line(line);
    }

    auto result = solver.get_result2();
    std::cout << "Result is " << result << std::endl;
    return 0;
}

} // task3