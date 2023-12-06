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

            if (is_symbol(ch)) {
                if (curr_number_opt) {
                    add_number_and_reset(curr_number_opt);
                }

                add_symbol(curr_x_pos, curr_y_pos);
                continue;
            }

            std::cerr << "We shouldn't be here." << std::endl;
            exit(1);
        }

        if (curr_number_opt) {
            add_number_and_reset(curr_number_opt);
        }

        ++line_count;
    }

    int get_result() {
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
    void handle_dot() {

    }

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

    void add_number_and_reset(std::optional<NumberDataHolder>& number) {
        if (!number) {
            std::cerr << "Optional is not initialized" << std::endl;
            exit(1);
        }

        const auto key = std::make_pair(number->x, number->y);
        if (numbers.contains(key)) {
            std::cerr << "Duplicate coords, cannot happen. " << std::endl;
            exit(1);
        }

        numbers[key] = number->get_value();
        number = std::nullopt;
    }

    void add_symbol(int x, int y) {
        const auto key = std::make_pair(x, y);
        if (symbols.contains(key)) {
            std::cerr << "Duplicate coords, cannot happen. " << std::endl;
            exit(1);
        }

        symbols.insert(key);
    }


private:
    static bool is_digit(char ch) {
        return isdigit(ch);
    }

    static bool is_dot(char ch) {
        return ch == '.';
    }

    static bool is_symbol(char ch) {
        return !is_digit(ch) && !is_dot(ch);
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

    typedef std::pair<int, int> coords;
    std::unordered_map<coords, int, hash_pair> numbers;
    std::unordered_set<coords, hash_pair> symbols;
};

int main() {
    std::ifstream input("3.input");
    if (!input) {
        std::cerr << "Can't open file.";
        return 1;
    }

    std::string line;
    Solver solver;
    while (std::getline(input, line)) {
        solver.add_line(line);
    }

    auto result = solver.get_result();
    std::cout << "Result is " << result << std::endl;
    return 0;
}

} // task3