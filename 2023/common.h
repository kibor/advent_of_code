#pragma once

#include <charconv>
#include <iomanip>
#include <iostream>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

namespace common {

template<typename T>
T parse_number(const std::string_view int_str) {
    T result = 0;
    auto [ptr, ec] = std::from_chars(int_str.data(), int_str.data() + int_str.length(), result);

    if (ec != std::errc()) {
        std::cerr << "Cannot parse number for string " << int_str << std::endl;
        exit(1);
    }

    return result;
    
}

std::vector<std::string_view> split_string(const std::string_view& str, const std::string& delim) {
    std::vector<std::string_view> result;
    for (const auto word : std::views::split(str, delim)) {
        std::string_view token(word.begin(), word.end());
        result.push_back(token);
    }

    return result;
}

std::string_view trim(const std::string_view sv) {
    auto view = sv
        | std::views::drop_while(isspace) 
        | std::views::reverse 
        | std::views::drop_while(isspace)
        | std::views::reverse;

    return std::string_view{view.begin().base().base(), view.end().base().base()};
}

std::string sv_to_string(const std::string_view sv) {
    return std::string(sv.begin(), sv.end());
}

} // common