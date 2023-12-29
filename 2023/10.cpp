#include <fstream>
#include <string>
#include <iostream>
#include <string_view>
#include <algorithm>
#include <vector>
#include <list>
#include <stdio.h>
#include <unordered_map>

#include "common.h"
#include "10.h"

namespace {

class Cell {
public:
    Cell(char cell) {
        set_cell(cell);
    }

public:
    bool can_go_north() const {
        return cell_ == '|'
            || cell_ == 'L'
            || cell_ == 'J';
    }

    bool can_go_east() const {
        return cell_ == '-'
            || cell_ == 'L'
            || cell_ == 'F';
    }

    bool can_go_south() const {
        return cell_ == '|'
            || cell_ == '7'
            || cell_ == 'F';
    }

    bool can_go_west() const {
        return cell_ == '-'
            || cell_ == '7'
            || cell_ == 'J';
    }

public:
    void set_cell(const char cell) {
        VERIFY(cell == '|' || cell == '-' || cell == 'L' 
                || cell == 'J' || cell == '7' || cell == 'F' 
                || cell == '.' || cell == 'S', << "Wrong cell type " << cell);

        cell_ = cell;
    }

private:
    char cell_;
};

class Solver {
public:
    void add_line(const std::string& line) {
        maze_.emplace_back();
        auto& new_row = maze_.back();

        for (size_t i = 0; i < line.size(); ++i) {
            char ch = line[i];
            new_row.emplace_back(ch);

            if (ch == 'S') {
                start_x_ = i;
                start_y_ = maze_.size() - 1;
            }
        }

    }

    long get_step_count() {
        update_start_cell();
        return 0;
    }


private:
    void update_start_cell() {
        VERIFY(start_x_ != -1 && start_y_ != -1, << "Start pos is not initialized");


    }


private:
    std::vector<std::vector<Cell>> maze_;
    size_t start_x_ = -1;
    size_t start_y_ = -1;

private:
    static const char START = 'S';
};


} // namespace

namespace task10 {

int main() {
    std::ifstream input("10.input");
    VERIFY(input, << "Can't open file");

    Solver solver;    

    std::string line;
    while (std::getline(input, line)) {
        solver.add_line(line);
    }

    std::cout << "Result is " << solver.get_step_count() << std::endl;
    return 0;
}


} // task10