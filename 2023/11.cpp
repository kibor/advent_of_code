#include <fstream>
#include <string>
#include <iostream>
#include <string_view>
#include <algorithm>
#include <vector>
#include <unordered_set>

#include "common.h"
#include "11.h"

namespace {

class Solver {
public:
    void add_line(const std::string& line, int line_number) {
        if (x_size_ == -1) {
            x_size_ = line.size();
        }

        VERIFY(x_size_ == line.size(), << "Wrong size");
        VERIFY(y_size_ + 1 == line_number, << "Wrong size");

        y_size_ = line_number;

        for (int x = 0; x < line.size(); ++x) {
            if (line[x] != GALAXY) {
                continue;
            }

            galaxies_.insert(std::make_pair(x, line_number));
        }

    }

    int get_sum_of_all_paths() {
        const auto empty_rows = find_empty_rows();
        const auto empty_cols = find_empty_cols();
        return 0;
    }

private:
    std::vector<int> find_empty_rows() const {
        return {};
    }

    std::vector<int> find_empty_cols() const {
        return {};
    }

private:
    static const char GALAXY = '#';

private:
    common::CoordSet<int> galaxies_;
    int x_size_ = -1;
    int y_size_ = -1;
};


} // namespace

namespace task11 {

int main() {
    std::ifstream input("11.input");
    VERIFY(input, << "Can't open file");

    Solver solver;    

    std::string line;
    int line_number = 0;
    while (std::getline(input, line)) {
        solver.add_line(line, line_number);
        ++line_number;
    }

    int result = solver.get_sum_of_all_paths();
    std::cout << "Result = " << result << std::endl;
    return 0;
}


} // task11