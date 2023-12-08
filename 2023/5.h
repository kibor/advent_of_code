#pragma once

#include <fstream>
#include <string>
#include <iostream>
#include <iterator>
#include <string_view>
#include <optional>
#include <ranges>
#include <unordered_set>
#include <unordered_map>

#include "common.h"

namespace task5 {

class Solver {
public:
    void add_line(const std::string& line) {
        if (line.starts_with("seeds:")) {
            handle_seeds(line);
            return;
        }

        if (line.empty()) {
            handle_break();
            return;
        }

        handle_mappings(line);
    }

    unsigned long get_location_by_seed(unsigned long seed) {
        std::string mapper_name = "seed";
        unsigned long value = seed;

        while (true) {
            auto& mapper = get_mapper(mapper_name);
            value = mapper.map_value(value);
            mapper_name = mapper.get_dest();

            if (mapper_name == "location") {
                return value;
            }
        }
    }

    unsigned long get_min_location() {
        unsigned long min_location = -1;
        for (unsigned long seed : seeds) {
            unsigned long location = get_location_by_seed(seed);
            std::cout << "Seed " << seed << " has location " << location << std::endl;

            if (min_location == -1 || min_location > location) {
                min_location = location;
            }
        }        

        return min_location;
    }

    unsigned long get_min_location2() {
        unsigned long min_location = -1;
        for (int i = 0; i < seeds.size(); i += 2) {
            for (int seed = seeds[i]; seed < seeds[i] + seeds[i + 1]; ++seed) {
                unsigned long location = get_location_by_seed(seed);
                std::cout << "Seed " << seed << " has location " << location << std::endl;

                if (min_location == -1 || min_location > location) {
                    min_location = location;
                }
            }
        }        

        return min_location;
    }


private:
    void handle_seeds(const std::string& line) {
        auto dots = std::ranges::find(line, ':');
        if (dots == line.end()) {
            std::cerr << "Can't find :" << std::endl;
            exit(1);
        }

        for (const auto seed : 
                common::split_string({dots + 1, line.end()}, " ")
                | std::views::transform(common::trim)
                | std::views::filter([](std::string_view sv) { return !sv.empty(); })
                | std::views::transform(common::parse_number<unsigned long>)
            ) {

            std::cout << "Pushing seed " << seed << std::endl;
            seeds.push_back(seed);
        }

    }

    void handle_break() {
        std::cout << "Hangling break" << std::endl;
        current_mapper = "";
    }

    class Mapper;
    Mapper& get_current_mapper() {
        return get_mapper(current_mapper);
    }

    void set_new_mapper(const std::string& line) {
        if (!current_mapper.empty()) {
            std::cerr << "Can't start new mapper until old is done" << std::endl;
            exit(1);
        }

        auto blank = std::ranges::find(line, ' ');
        if (blank == line.end()) {
            std::cerr << "Wrong format for name" << std::endl;
            exit(1);
        }

        const auto mappings = common::split_string({line.begin(), blank}, "-") 
                            | std::views::transform(common::sv_to_string);

        if (mappings.size() != 3) {
            std::cerr << "Wrong format for name" << std::endl;
            exit(1);
        }

        const std::string source = mappings[0];
        const std::string dest = mappings[2];

        if (mappers.contains(source)) {
            std::cerr << "Mapper already exists " << std::endl;
            exit(1);
        }

        std::cout << "Creating new mapper from " << source << " to " << dest << std::endl;
        mappers[source] = Mapper(source, dest);
        current_mapper = source;
    }

    void handle_mappings(const std::string& line) {
        std::cout << "Handling mappings" << std::endl;

        auto dots = std::ranges::find(line, ':');
        if (dots != line.end()) {
            set_new_mapper(line);
            return;
        }

        std::vector<unsigned long> values;
        for (auto value : common::split_string(line, " ") | std::views::transform(common::parse_number<unsigned long>)) {
            std::cout << "Adding new mapping value " << value << std::endl;
            values.push_back(value);
        }

        if (values.size() != 3) {
            std::cerr << "Wrong range format" << std::endl;
            exit(1);
        }

        get_current_mapper().add_range(values[0], values[1], values[2]);
    }

    Mapper& get_mapper(const std::string& name) {
        auto it = mappers.find(name);
        if (it == mappers.end()) {
            std::cerr << "Can't find mapper " << name << std::endl;
            exit(1);
        }

        return it->second;
    }

private:
    struct MappingRange {
        unsigned long dest_range_start;
        unsigned long source_range_start;
        unsigned long range;
    };

    class Mapper {
    public:
        Mapper() {}

        Mapper(const std::string& source, const std::string& dest)
        : source(source), dest(dest)
        {}

        void add_range(unsigned long dest_range_start, unsigned long source_range_start, unsigned long range) {
            ranges.emplace_back(dest_range_start, source_range_start, range);
        }

    public:
        const std::string& get_source() const { return source; }
        const std::string& get_dest() const { return dest; }

        unsigned long map_value(unsigned long source) const {
            for (const auto& range : ranges) {
                if (source >= range.source_range_start && source < range.source_range_start + range.range) {
                    return range.dest_range_start + (source - range.source_range_start);
                }
            }
            return source;
        }

    private:
        std::string source;
        std::string dest;
        std::vector<MappingRange> ranges;
    };

private:
    std::vector<unsigned long> seeds;
    std::string current_mapper;
    std::unordered_map<std::string, Mapper> mappers;
};

unsigned long main() {
    std::ifstream input("5.input");

    if (!input) {
        std::cerr << "Can't open file.";
        return 1;
    }

    Solver solver;
    std::string line;
    while (std::getline(input, line)) {
        solver.add_line(line);
    }

    unsigned long min_location = solver.get_min_location2();
    std::cout << "min location is " << min_location << std::endl;

    return 0;
}

} // task5