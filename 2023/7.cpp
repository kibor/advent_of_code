#include <array>
#include <fstream>
#include <string>
#include <iostream>
#include <string_view>
#include <algorithm>
#include <tuple>
#include <unordered_map>

#include "common.h"
#include "7.h"

namespace {

std::tuple<std::string, int> parse_line(const std::string& line) {
    auto values = common::split_string(line, " ");
    VERIFY(values.size() == 2, << "Wrong format");

    return std::make_tuple(std::string(values[0]), common::parse_number<int>(values[1]));
}

class Hand {
public:
    Hand() = default;
    Hand(std::string hand, int bid) : hand_(hand), bid_(bid) {}
    Hand& operator=(const Hand& hand) = default;

public:
    auto operator <=> (const Hand& card) const {
        if (hand_strength() != card.hand_strength()) {
            return hand_strength() <=> card.hand_strength();
        }

        for (int i = 0; i < hand_.length(); ++i) {
            if (card_strength(hand_[i]) != card_strength(card.hand_[i])) {
                return card_strength(hand_[i]) <=> card_strength(card.hand_[i]);
            }
        }

        return std::strong_ordering::equal;
    }

private:
    static const int FIVE_OF_A_KIND = 6;
    static const int FOUR_OF_A_KIND = 5;
    static const int FULL_HOUSE = 4;
    static const int THREE_OF_A_KIND = 3;
    static const int TWO_PAIRS = 2;
    static const int ONE_PAIR = 1;
    static const int HIGH_CARD = 0;

private:
    int hand_strength() const {
        std::unordered_map<char, int> kinds;
        for (char ch : hand_) {
            ++kinds[ch];
        }

        const size_t size = kinds.size();

        if (size == 1) {
            return FIVE_OF_A_KIND;
        }

        if (size == 5) {
            return HIGH_CARD;
        }

        if (size == 4) {
            return ONE_PAIR;
        }

        if (size == 2) {
            auto value = kinds.begin()->second;
            return value == 1 || value == 4 ? FOUR_OF_A_KIND : FULL_HOUSE;
        }

        for (auto [key, value] : kinds) {
            if (value == 3) {
                return THREE_OF_A_KIND;
            }
        }

        return TWO_PAIRS;
    }

    static size_t card_strength(char ch) {
        static const std::array<char, 13> cards = {'2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A'};

        auto it = std::ranges::find(cards, ch);
        VERIFY(it != cards.end(), << "Card doesn't exist");

        return std::distance(cards.begin(), it);
    }

public:
    std::string hand() const { return hand_; }
    int bid() const { return bid_; }


private:
    std::string hand_;
    int bid_;
};

} // namespace

namespace task7 {

int main() {
    std::ifstream input("7.input");
    VERIFY(input, << "Can't open file");

    std::vector<Hand> hands; 
    std::string line;
    while (std::getline(input, line)) {
        auto [hand, bid] = parse_line(line);
        std::cout << "Hand is " << hand << ", bid is " << bid << std::endl;

        hands.emplace_back(hand, bid);
    }

    unsigned long result = 0;
    std::ranges::sort(hands, std::less());
    for (int i = 0; i < hands.size(); ++i) {
        const auto& hand = hands[i];
        std::cout << "Hand " << hand.hand() << " has rank " << i + 1 << " and bid = " << hand.bid() << std::endl;
        result += (i + 1) * hand.bid(); 
    }
    
    std::cout << "Result is " << result << std::endl;
    return 0;
}


} // task7