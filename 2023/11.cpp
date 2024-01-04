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
    void add_line(const std::string& line, long line_number) {
        if (x_size_ == -1) {
            x_size_ = line.size();
        }

        VERIFY(x_size_ == line.size(), << "Wrong size");
        VERIFY(y_size_ + 1 == line_number, << "Wrong size");

        y_size_ = line_number;

        for (long x = 0; x < line.size(); ++x) {
            if (line[x] != GALAXY) {
                continue;
            }

            galaxies_.emplace_back(x, line_number);
        }
    }

    long long get_sum_of_all_paths() {
        const auto empty_rows = find_empty_rows();
        const auto empty_cols = find_empty_cols();
        update_coords(empty_rows, empty_cols);
        print_coords();

        return calc_distances();
    }

private:
    long long calc_distances() const {
        long long result = 0;

        for (const auto& g1 : galaxies_) {
            for (const auto& g2 : galaxies_) {
                result += std::abs(g1.first - g2.first) + std::abs(g1.second - g2.second);
            }
        }

        return result / 2;
    }

    void print_coords() const {
        for (const auto& g : galaxies_) {
            std::cout << "x = " << g.first << ", y = " << g.second << std::endl;
        }
    }

    void update_coords(const std::unordered_set<long>& empty_rows, const std::unordered_set<long>& empty_cols) {
        for (const long row : std::ranges::reverse_view(empty_rows)) {
            for (auto& galaxy : galaxies_) {
                if (galaxy.first > row) {
                    galaxy.first += 999999;
                }
            }
        }

        for (const long col : std::ranges::reverse_view(empty_cols)) {
            for (auto& galaxy : galaxies_) {
                if (galaxy.second > col) {
                    galaxy.second += 999999;
                }
            }
        }
    }

    std::unordered_set<long> find_empty_rows() const {
        std::unordered_set<long> rows;
        for (long x = 0; x < x_size_; ++x) {
            rows.insert(x);
        }

        for (const auto& galaxy : galaxies_) {
            rows.erase(galaxy.first);
        }

        return rows;
    }

    std::unordered_set<long> find_empty_cols() const {
        std::unordered_set<long> cols;
        for (long y = 0; y < y_size_; ++y) {
            cols.insert(y);
        }

        for (const auto& galaxy : galaxies_) {
            cols.erase(galaxy.second);
        }

        return cols;
    }

private:
    static const char GALAXY = '#';

private:
    std::vector<common::Coords<long>> galaxies_;
    long x_size_ = -1;
    long y_size_ = -1;
};


} // namespace

namespace task11 {

int main() {
    std::ifstream input("11.input");
    VERIFY(input, << "Can't open file");

    Solver solver;    

    std::string line;
    long line_number = 0;
    while (std::getline(input, line)) {
        solver.add_line(line, line_number);
        ++line_number;
    }

    auto result = solver.get_sum_of_all_paths();
    std::cout << "Result = " << result << std::endl;
    return 0;
}

} // task11