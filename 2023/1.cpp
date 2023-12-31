#include <fstream>
#include <string>
#include <iostream>
#include <iterator>
#include <optional>
#include <ranges>

#include "common.h"
#include "1.h"

namespace {

template<typename TIter>
int find_digit(TIter begin, TIter end) {
    auto criteria = [](char ch) { return ch >= '0' && ch <= '9'; };

    auto it = std::ranges::find_if(begin, end, criteria);
    VERIFY(it != end, << "Can't find digit");

    return *it - '0';
}

int get_number(const std::string& line) {
    int first_digit = find_digit(line.begin(), line.end());
    int second_digit = find_digit(line.rbegin(), line.rend());

    return 10 * first_digit + second_digit;
}

class digital_data {
public:
    digital_data() {
        start_index = -1;
        end_index = -1;
        start_digit = -1;
        end_digit = -1;
    }

    void add_digit(int digit, int index) {
        if (start_index == -1 || start_index > index) {
            start_index = index;
            start_digit = digit;
        }
        if (end_index == -1 || end_index < index) {
            end_index = index;
            end_digit = digit;
        }
    }

    int get_start_index() const { return start_index; }
    int get_end_index() const { return end_index; }

    int get_number() const {
        VERIFY(start_index != -1 && end_index != -1, << "Indexes not initialized");
        return 10 * start_digit + end_digit;
    }

private:
    int start_index;
    int end_index;
    int start_digit;
    int end_digit;
};

void look_for_letter_digit(const std::string& line, const std::string& letter_digit, int digit, digital_data& dd) {
    int i = 0;
    while (true) {
        // Skip data that we don't care about.
        if (i >= dd.get_start_index() && i <= dd.get_end_index()) {
            i = dd.get_end_index() + 1;
        }

        i = line.find(letter_digit, i);
        if (i == std::string::npos) {
            return;
        }

        dd.add_digit(digit, i);
        i += letter_digit.length();
    }
}


int get_number2(const std::string& line) {
    digital_data dd;

    for (int i = 0; i < line.length(); ++i) {
        char ch = line[i];
        if (ch >= '0' && ch <= '9') {
            dd.add_digit(ch - '0', i);
            continue;
        }
    }

    look_for_letter_digit(line, "one", 1, dd);
    look_for_letter_digit(line, "two", 2, dd);
    look_for_letter_digit(line, "three", 3, dd);
    look_for_letter_digit(line, "four", 4, dd);
    look_for_letter_digit(line, "five", 5, dd);
    look_for_letter_digit(line, "six", 6, dd);
    look_for_letter_digit(line, "seven", 7, dd);
    look_for_letter_digit(line, "eight", 8, dd);
    look_for_letter_digit(line, "nine", 9, dd);

    return dd.get_number();
}

} // namespace

namespace task1 {

int main()
{
    std::ifstream input("1.input");
    VERIFY(input, << "Can't open file");

    std::string line;
    long result = 0;
    while (std::getline(input, line))
    {
        int number = get_number2(line);
        result += number;
    }

    std::cout << "Result is " << result << std::endl;
    return 0;
}

} // task1