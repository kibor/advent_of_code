#pragma once

#include <fstream>
#include <string>
#include <iostream>
#include <iterator>
#include <optional>
#include <ranges>
#include <ctype.h>

class Solver {
public:
    void add_line(const std::string& line) {
        int curr_y_pos = line_count;
        int curr_x_pos = -1;
        std::optional<NumberDataHolder> curr_number_opt;

        std::cout << "Processing line " << curr_y_pos << std::endl;
        for (char ch : line) {
            ++curr_x_pos;

            std::cout << "x = " << curr_x_pos << ", ch = " << ch << std::endl; 

            if (is_dot(ch)) {
                std::cout << "Found a dot. ";
                if (!curr_number_opt) {
                    std::cout << "Not processing numbers, so just continue." << std::endl;
                    continue;
                }

                add_number(*curr_number_opt);
                curr_number_opt = std::nullopt;
                
                continue;
            }

            if (is_digit(ch)) {
                std::cout << "Found new digit " << ch << ". ";
                if (!curr_number_opt) {
                    std::cout << "It is a new number." << std::endl;
                    curr_number_opt = NumberDataHolder(ch, curr_x_pos, curr_y_pos);
                    
                    continue;
                }

                std::cout << "Adding digit to the current number." << std::endl;
                curr_number_opt->add_digit(ch);

                continue;
            }

            if (is_symbol(ch)) {
                std::cout << "Found symbol. ";
                if (!curr_number_opt) {
                    add_symbol(curr_x_pos, curr_y_pos);

                    continue;
                }

                add_number(*curr_number_opt);
                curr_number_opt = std::nullopt;
                
                continue;
            }

            std::cerr << "We shouldn't be here." << std::endl;
            exit(1);
        }

        if (curr_number_opt) {
            add_number(*curr_number_opt);
            curr_number_opt = std::nullopt;
        }

        ++line_count;
    }

    int get_result() const {
        return 0;
    }

private:
    struct NumberDataHolder {
        int x;
        int y;
        std::string value;

        NumberDataHolder(char ch, int x, int y) {
            this->x = x;
            this->y = y;
            value = ch;
        }

        void add_digit(char ch) {
            value += ch;
        }

        int get_value () const {
            return std::stoi(value);
        }
    };

private:
    void add_number(const NumberDataHolder& number) {
        std::cout << "Adding new number " << number.get_value() 
            << ", x = " << number.x << ", y = " << number.y << std::endl;
    }

    void add_symbol(int x, int y) {
        std::cout << "Adding new symbol, x = " << x << ", y = " << y << std::endl;
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

private:
    int line_count = 0;
};

int main3() {
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

    std::cout << "Result is " << solver.get_result() << std::endl;
    return 0;
}