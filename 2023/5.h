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
#include <tuple>

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

    void optimize_mappers() {
        for (auto& [mapper_name, mapper] : mappers) {
            mapper.optimize();
        }

    }

private:
    void handle_seeds(const std::string& line) {
        auto dots = std::ranges::find(line, ':');
        VERIFY(dots != line.end(), << "Can't find dots");

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


    class Mapper;
    Mapper& get_current_mapper() {
        return get_mapper(current_mapper);
    }

    void set_new_mapper(const std::string& line) {
        VERIFY(current_mapper.empty(), << "Can't start new mapper until old is done");

        const auto [source, dest] = extract_source_and_dest(line);
        VERIFY(!mappers.contains(source), << "Mapper already exists ");

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

        VERIFY(values.size() == 3, << "Wrong range format");
        get_current_mapper().add_range(values[0], values[1], values[2]);
    }

    Mapper& get_mapper(const std::string& name) {
        auto it = mappers.find(name);
        VERIFY(it != mappers.end(), << "Can't find mapper");

        return it->second;
    }

    static std::tuple<std::string, std::string> extract_source_and_dest(const std::string& line) {
        auto blank = std::ranges::find(line, ' ');
        VERIFY(blank != line.end(), << "Wrong format for name " << line);

        const auto mappings = common::split_string({line.begin(), blank}, "-") 
                            | std::views::transform(common::sv_to_string);

        VERIFY(mappings.size() == 3, << "Wrong format for name. Size must be 3, but it is " << mappings.size());
        return std::make_tuple(mappings[0], mappings[2]);
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

        void optimize() {
            auto compare = [](const MappingRange& lhs, const MappingRange& rhs) { return lhs.source_range_start < rhs.source_range_start; };
            std::sort(ranges.begin(), ranges.end(), compare);

            std::cout << "Mapper from " << source << " to " << dest << " has ranges: " << std::endl;
            for (const auto& r : ranges) {
                std::cout << "\tsource range: " << r.source_range_start << " - " << r.source_range_start + r.range - 1
                    << ", dest range: " << r.dest_range_start << " - " << r.dest_range_start + r.range - 1 << std::endl;
            }
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
    VERIFY(input, << "Can't open file");

    Solver solver;
    std::string line;
    while (std::getline(input, line)) {
        solver.add_line(line);
    }

    solver.optimize_mappers();
    return 0;

    unsigned long min_location = solver.get_min_location2();
    std::cout << "min location is " << min_location << std::endl;

    return 0;
}

} // task5