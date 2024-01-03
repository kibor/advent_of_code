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

};


} // namespace

namespace task11 {

int main() {
    std::ifstream input("11.input");
    VERIFY(input, << "Can't open file");

    Solver solver;    

    //std::string line;
    //while (std::getline(input, line)) {
    //    solver.add_line(line);
    //}

    //int result = solver.get_step_count();
    //std::cout << "Result = " << result << std::endl;
    return 0;
}


} // task11