#include <fstream>
#include <string>
#include <iostream>
#include <string_view>
#include <algorithm>
#include <vector>
#include <list>
#include <stdio.h>
#include <unordered_set>
#include <tuple>
#include <optional>

#include "common.h"
#include "10.h"

namespace {

using Coords = common::Coords<int>;
using CoordSet = common::CoordSet<int>;

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

    char get_cell() const {
        return cell_;
    }

private:
    char cell_;
};

class Solver {
public:
    void add_line(const std::string& line) {
        maze_.emplace_back();
        auto& new_row = maze_.back();

        for (int i = 0; i < line.size(); ++i) {
            char ch = line[i];
            new_row.emplace_back(ch);

            if (ch == 'S') {
                start_x_ = i;
                start_y_ = maze_.size() - 1;
            }
        }

    }

    long get_step_count(){
        update_start_cell();
        const auto visited = traverse_loop();
        const auto [external_cells, internal_cells] = mark_cells(visited);
        print_maze(visited, external_cells, internal_cells);

        std::cout << "Steps count = " << visited.size() 
            << ", external cells count = " << external_cells.size()
            << ", internal cells count = " << internal_cells.size() << std::endl;
        VERIFY(visited.size() % 2 == 0, << "Wrong amount of steps");
        return visited.size() / 2;
    }

private:
    CoordSet traverse_loop() const {
        CoordSet visited;

        int x = start_x_;
        int y = start_y_;

        while (true) {
            const auto coord = std::make_pair(x, y);
            VERIFY(!visited.contains(coord), << "Already visited x = " << x << " y = " << y);

            visited.insert(coord);
            const Cell& current_cell = get_cell(x, y);

            if (current_cell.can_go_north() && !visited.contains(std::make_pair(x, y - 1))) {
                y = y - 1;
                continue;
            }

            if (current_cell.can_go_east() && !visited.contains(std::make_pair(x + 1, y))) {
                x = x + 1;
                continue;
            }

            if (current_cell.can_go_south() && !visited.contains(std::make_pair(x, y + 1))) {
                y = y + 1;
                continue;
            }

            if (current_cell.can_go_west() && !visited.contains(std::make_pair(x - 1, y))) {
                x = x - 1;
                continue;
            }

            std::cout << "Loop is done" << std::endl;
            break;
        }

        return visited;
    }

    std::tuple<CoordSet, CoordSet> mark_cells(const CoordSet& visited) const {
        CoordSet external_cells;
        CoordSet internal_cells;

        for (int y = 0; y < maze_.size(); ++y) {
            const auto& row = maze_[y];
            for (int x = 0; x < row.size(); ++x) {
                const auto coord = std::make_pair(x, y);

                if (visited.contains(coord)) {
                    continue;
                }

                if (is_external(visited, x, y, row.size(), maze_.size())) {
                    external_cells.insert(coord);
                } else {
                    internal_cells.insert(coord);
                }
            }

            std::cout << std::endl;
        }

        return std::make_tuple(external_cells, internal_cells);
    }

    static bool has_north_south_intersection(const Cell& c1, const Cell& c2) {
        return c1.can_go_north() && c2.can_go_south()
            || c2.can_go_north() && c1.can_go_south();
    }

    bool is_external(const CoordSet& visited, const int x, const int y, const int x_size, const int y_size) const {
        int intersections = 0;
        std::optional<Cell> prev_cell;

        // From left border to x
        for (int x1 = x - 1; x1 >= 0; --x1) {
            const auto& cell = get_cell(x1, y);
            if (!visited.contains(std::make_pair(x1, y))) {
                continue;
            }

            if (cell.can_go_north() && cell.can_go_south()) {
                ++intersections;
                continue;
            }

            if (cell.can_go_east() && cell.can_go_west()) {
                continue;
            }

            if (!prev_cell) {
                prev_cell = cell;
                continue;
            }

            if (has_north_south_intersection(cell, *prev_cell)) {
                    ++intersections;
            }
            
            prev_cell.reset();
        }

        return intersections % 2 == 0;
    }

    void print_maze(const CoordSet& visited, const CoordSet& external_cells, const CoordSet& internal_cells) const {
        std::cout << "Maze: " << std::endl;

        for (int y = 0; y < maze_.size(); ++y) {
            const auto& row = maze_[y];
            for (int x = 0; x < row.size(); ++x) {
                const auto coord = std::make_pair(x, y);
                if (visited.contains(coord)) {
                    std::cout << '#';
                }
                else if (external_cells.contains(coord)) {
                    std::cout << 'O';
                }
                else if (internal_cells.contains(coord)) {
                    std::cout << 'I';
                }
                else {
                    FAIL(<< "We cannot be here");
                }
            }

            std::cout << std::endl;
        }
    }


private:
    void update_start_cell() {
        VERIFY(start_x_ != -1 && start_y_ != -1, << "Start pos is not initialized");

        Cell north_cell = get_cell(start_x_, start_y_ - 1);
        Cell east_cell = get_cell(start_x_ + 1, start_y_);
        Cell south_cell = get_cell(start_x_, start_y_ + 1);
        Cell west_cell = get_cell(start_x_ - 1, start_y_);

        char new_value = GROUND;

        if (north_cell.can_go_south() && south_cell.can_go_north()) {
            VERIFY(new_value == GROUND, << "new value already updated");
            new_value = '|';
        }

        if (west_cell.can_go_east() && east_cell.can_go_west()) {
            VERIFY(new_value == GROUND, << "new value already updated");
            new_value = '-';
        }

        if (north_cell.can_go_south() && east_cell.can_go_west()) {
            VERIFY(new_value == GROUND, << "new value already updated");
            new_value = 'L';
        }

        if (north_cell.can_go_south() && west_cell.can_go_east()) {
            VERIFY(new_value == GROUND, << "new value already updated");
            new_value = 'J';
        }

        if (west_cell.can_go_east() && south_cell.can_go_north()) {
            VERIFY(new_value == GROUND, << "new value already updated");
            new_value = '7';
        }

        if (east_cell.can_go_west() && south_cell.can_go_north()) {
            VERIFY(new_value == GROUND, << "new value already updated");
            new_value = 'F';
        }

        VERIFY(new_value != GROUND, << "new value has not been updated");
        
        std::cout << "Starting point is " << new_value << std::endl;
        maze_[start_y_][start_x_] = new_value;
    }

    Cell get_cell(int x, int y) const {
        if (y < 0 || y >= maze_.size()) {
            return Cell(GROUND);
        }

        const auto& row = maze_[y];
        if (x < 0 || x >= row.size()) {
            return Cell(GROUND);
        }

        return Cell(row[x]);
    }

private:
    std::vector<std::vector<Cell>> maze_;
    int start_x_ = -1;
    int start_y_ = -1;

private:
    static const char START = 'S';
    static const char GROUND = '.';
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

    int result = solver.get_step_count();
    std::cout << "Result = " << result << std::endl;
    return 0;
}


} // task10