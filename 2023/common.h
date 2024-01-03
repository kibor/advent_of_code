#pragma once

#include <charconv>
#include <iomanip>
#include <iostream>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace common {

#define VERIFY(x, msg) if (!(x)) { std::cerr msg << std::endl; exit(1); }

#define FAIL(msg) VERIFY(false, msg) 

template<typename T>
inline T parse_number(const std::string_view int_str) {
    T result = 0;
    auto [ptr, ec] = std::from_chars(int_str.data(), int_str.data() + int_str.length(), result);

    VERIFY(ec == std::errc(), << "Cannot parse number for string " << int_str);
    return result;    
}

inline std::vector<std::string_view> split_string(const std::string_view& str, const std::string& delim) {
    std::vector<std::string_view> result;
    for (const auto word : std::views::split(str, delim)) {
        std::string_view token(word.begin(), word.end());
        result.push_back(token);
    }

    return result;
}

inline std::string_view trim(const std::string_view sv) {
    auto view = sv
        | std::views::drop_while(isspace) 
        | std::views::reverse 
        | std::views::drop_while(isspace)
        | std::views::reverse;

    return std::string_view{view.begin().base().base(), view.end().base().base()};
}

inline std::string sv_to_string(const std::string_view sv) {
    return std::string(sv.begin(), sv.end());
}


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

template<typename T> using Coords = std::pair<T, T>;
template<typename T> using CoordSet = std::unordered_set<Coords<T>, hash_pair>;
template<typename K, typename V> using CoordMap = std::unordered_map<Coords<K>, V, hash_pair>;
} // common