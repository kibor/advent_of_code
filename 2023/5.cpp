#include <fstream>
#include <string>
#include <iostream>
#include <string_view>
#include <unordered_map>
#include <tuple>
#include <algorithm>
#include <limits>

#include "common.h"
#include "5.h"

namespace {

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
            auto input_range = std::make_pair(seeds[i], seeds[i + 1]);

            while(input_range.second != 0) {
                auto seed = input_range.first;
                auto location = get_location_by_seed(seed);
                std::cout << "Min location for seed " << seed  << " is " << location << std::endl;

                if (min_location == -1 || min_location > location) {
                    min_location = location;
                }

                auto new_range = get_min_range(input_range);
                input_range.first += new_range;

                VERIFY(input_range.second >= new_range, << "Can never happen");
                input_range.second -= new_range;
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

    typedef std::pair<unsigned long, unsigned long> Range;

    unsigned long get_min_range(const Range& seeds_range) {
        std::string mapper_name = "seed";
        auto result = seeds_range;

        while (true) {
            auto& mapper = get_mapper(mapper_name);

            std::cout << "Mapping " << mapper_name << " to " << mapper.get_dest() << std::endl;
            std::cout << "Input range is from " << result.first << " to "
                << result.first + result.second - 1
                << ", range length is " << result.second
                << std::endl;

            result = mapper.get_minimal_range(result);

            std::cout << "Output range is from " << result.first << " to "
                << result.first + result.second - 1
                << ", range length is " << result.second
                << std::endl;

            mapper_name = mapper.get_dest();

            if (mapper_name == "location") {
                return result.second;
            }
        }
    }

private:
    struct MappingRange {
        unsigned long dest_range_start;
        unsigned long source_range_start;
        unsigned long range;

        bool contains_source(unsigned long p) const {
            return source_range_start <= p && source_range_start + range > p;
        }

        unsigned long map_to_range(unsigned long source) const {
            return dest_range_start + (source - source_range_start);
        }
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

        unsigned long map_value(unsigned long s) const {
            for (const auto& r : ranges) {
                if (r.contains_source(s)) {
                    return r.map_to_range(s);
                }
            }

            return s;
        }

        Range get_minimal_range(const Range& input_range) const {
            for (const auto& r : ranges) {
                std::cout << "Checking if " << input_range.first << " is between "
                    << r.source_range_start << " and " << r.source_range_start + r.range << std::endl;

                if (!r.contains_source(input_range.first)) {
                    continue;
                }

                std::cout << "Checking successful" << std::endl;

                auto range_start = r.map_to_range(input_range.first);
                auto range_end = std::min(input_range.second, r.dest_range_start + r.range - range_start);
                return std::make_pair(range_start, range_end);
            }

            VERIFY(false, << "We can't be here");
        }

        void optimize() {
            VERIFY(!ranges.empty(), << "Ranges can't be empty");

            auto compare = [](const MappingRange& lhs, const MappingRange& rhs) { return lhs.source_range_start < rhs.source_range_start; };
            std::ranges::sort(ranges, compare);

            const auto& last = *ranges.rbegin();
            if (last.source_range_start + last.range < std::numeric_limits<unsigned long>::max()) {
                add_range(last.source_range_start + last.range, last.source_range_start + last.range,
                    std::numeric_limits<unsigned long>::max() - last.source_range_start - last.range);
            }

            const auto& first = *ranges.begin();
            if (first.source_range_start != 0) {
                add_range(0, 0, first.source_range_start);
            }

            std::ranges::sort(ranges, compare);

            std::cout << "Mapper from " << source << " to " << dest << " has ranges: " << std::endl;
            for (const auto& r : ranges) {
                std::cout << "\tsource range: " << r.source_range_start << " - " << r.source_range_start + r.range - 1
                    << ", dest range: " << r.dest_range_start << " - " << r.dest_range_start + r.range - 1 << std::endl;
            }

            for (int i = 0; i < ranges.size() - 1; ++i) {
                if (ranges[i].source_range_start + ranges[i].range != ranges[i + 1].source_range_start) {
                    std::cerr << "Broken range" << std::endl;
                    auto new_start = ranges[i].source_range_start + ranges[i].range;

                    add_range(new_start, new_start, ranges[i + 1].source_range_start - new_start);
                    std::ranges::sort(ranges, compare);

                    optimize();
                }
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

} // namespace

namespace task5 {

int main() {
    std::ifstream input("5.input");
    VERIFY(input, << "Can't open file");

    Solver solver;
    std::string line;
    while (std::getline(input, line)) {
        solver.add_line(line);
    }

    solver.optimize_mappers();
    auto result = solver.get_min_location2();

    std::cout << "Min location is " << result << std::endl;
    return 0;
}

} // task5