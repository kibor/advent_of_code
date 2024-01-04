#include <fstream>
#include <string>
#include <iostream>
#include <string_view>
#include <algorithm>
#include <vector>
#include <unordered_set>

#include "common.h"
#include "12.h"

namespace {

class Solver {
public:
    void add_line(const std::string& line) {
        const auto& parts = common::split_string(line, " ");
        VERIFY(parts.size() == 2, << "Incorrect line");
    }

    long get_sum_of_all_options() {
        return 0;
    }

private:
    class Entry {
    public:
        Entry(const std::string& pattern, const std::string& groups) {

        }

    private:
    };

};


} // namespace

namespace task12 {

int main() {
    std::ifstream input("12.input");
    VERIFY(input, << "Can't open file");

    Solver solver;    

    std::string line;
    while (std::getline(input, line)) {
        solver.add_line(line);
    }

    auto result = solver.get_sum_of_all_options();
    std::cout << "Result = " << result << std::endl;
    return 0;
}

} // task11