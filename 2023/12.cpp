#include <fstream>
#include <string>
#include <ranges>
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
    int add_line(const std::string& line) {
        std::cout << "Processing line " << line << std::endl;
        const auto& parts = common::split_string(line, " ");
        VERIFY(parts.size() == 2, << "Incorrect line");

        Entry entry(parts[0], parts[1]);
        return entry.get_all_variations();
    }

private:
    class Entry {
    public:
        Entry(const std::string_view& pattern, const std::string_view& groups) {
            pattern_ = duplicate(pattern, '?', 5);
            init_unknown_indexes(pattern_);

            const std::string duplicated_groups = duplicate(groups, ',', 5);
            init_breakage_groups(duplicated_groups);

            VERIFY(known_breakage_count_ <= total_breakage_count_, 
                << "Incorrect invariant. known = " << known_breakage_count_ << ", total = " << total_breakage_count_);
            VERIFY(known_breakage_count_ + unknown_indexes_.size() >= total_breakage_count_, << "Another incorrect invariant");
        }

        int get_all_variations() const {
            if (known_breakage_count_ == total_breakage_count_) {
                return 1;
            }

            int breakages = total_breakage_count_ - known_breakage_count_;
            std::string breakage_pattern;
            breakage_pattern.resize(unknown_indexes_.size());

            for (int i = 0; i < breakages; ++i) {
                breakage_pattern[i] = BROKEN;
            }

            for (int i = breakages; i < breakage_pattern.size(); ++i) {
                breakage_pattern[i] = WORKING;
            }

            int count = 0;
            do {
                const auto new_option = generate_option(breakage_pattern);
                if (is_acceptable(new_option)) {
                    ++count;
                }
            } while (std::next_permutation(breakage_pattern.begin(), breakage_pattern.end()));

            std::cout << "Total number of options for " << pattern_ << " is " << count << std::endl;

            return count;
        }

    private:
        static std::string duplicate(const std::string_view& pattern, const char delimeter, const int count) {
            std::string result(pattern.begin(), pattern.end());
            for (int i = 1; i < count; ++i) {
                result += delimeter;
                result += pattern;
            }

            return result;
        }

        std::string generate_option(const std::string& breakage_pattern) const {
            std::string result(pattern_);
            for (int i = 0; i < breakage_pattern.size(); ++i) {
                int index = unknown_indexes_[i];
                result[index] = breakage_pattern[i];
            }

            return result;
        }

        bool is_acceptable(const std::string& option) const {
            auto groups = common::split_string(option, std::string(1, WORKING))
                | std::views::transform(common::trim)
                | std::views::filter([](const std::string_view& str) { return !str.empty(); });

            int index = 0;
            for (const auto& group : groups) {
                if (index >= breakage_groups_.size()) {
                    return false;
                }

                if (group.size() != breakage_groups_[index]) {
                    return false;
                }

                ++index;
            }

            return breakage_groups_.size() >= index;

        }

        void init_unknown_indexes(const std::string_view& pattern) {
            for (int i = 0; i < pattern.length(); ++i) {
                char ch = pattern[i];
                if (ch == UNKNOWN) {
                    unknown_indexes_.push_back(i);
                }
                if (ch == BROKEN) {
                    ++known_breakage_count_;
                }
            }

            VERIFY(!unknown_indexes_.empty(), << "Cannot be empty");
        }

        void init_breakage_groups(const std::string_view& groups) {
            total_breakage_count_ = 0;
            breakage_groups_.clear();

            for (const auto& group : common::split_string(groups, ",")) {
                int number = common::parse_number<int>(group);
                breakage_groups_.push_back(number);
                total_breakage_count_ += number;
            }
        }

    private:
        static const char UNKNOWN = '?';
        static const char BROKEN = '#';
        static const char WORKING = '.';

    private:
        std::string pattern_;
        std::vector<int> unknown_indexes_;
        std::vector<int> breakage_groups_;
        int known_breakage_count_;
        int total_breakage_count_;
    };

};


} // namespace

namespace task12 {

int main() {
    std::ifstream input("12.input");
    VERIFY(input, << "Can't open file");

    Solver solver;    

    std::string line;
    long result = 0;
    while (std::getline(input, line)) {
        result += solver.add_line(line);
    }

    std::cout << "Result = " << result << std::endl;
    return 0;
}

} // task11