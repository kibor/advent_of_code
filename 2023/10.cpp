#include <fstream>
#include <string>
#include <iostream>
#include <string_view>
#include <algorithm>
#include <vector>
#include <list>
#include <stdio.h>
#include <unordered_set>

#include "common.h"
#include "10.h"

namespace {

struct hash_pair {
    template <class T1, class T2>
    size_t operator()(const std::pair<T1, T2>& p) const
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

typedef std::pair<int, int> coords;
typedef std::unordered_set<coords, hash_pair> Coords;

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

    long get_step_count() {
        update_start_cell();
        const auto visited = traverse_loop();
        print_maze(visited);

        std::cout << "Steps count = " << visited.size() << std::endl; 
        VERIFY(visited.size() % 2 == 0, << "Wrong amount of steps");
        return visited.size() / 2;
    }

private:
    Coords traverse_loop() {
        Coords visited;

        int x = start_x_;
        int y = start_y_;

        while(true) {
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

    void print_maze(const Coords& visited) {
        std::cout << "Maze: " << std::endl;

        for (int y = 0; y < maze_.size(); ++y) {
            const auto& row = maze_[y];
            for (int x = 0; x < row.size(); ++x) {
                if (!visited.contains(std::make_pair(x, y))) {
                    std::cout << '.';
                } else {
                    std::cout << '#';
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
    std::cout << "Result is " << result << std::endl;
    return 0;
}


} // task10