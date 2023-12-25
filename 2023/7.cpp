#include <array>
#include <fstream>
#include <string>
#include <iostream>
#include <string_view>
#include <algorithm>
#include <tuple>
#include <unordered_map>
#include <compare>

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
    Hand(const std::string& hand, int bid) : hand_(hand), bid_(bid) {}
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

        FAIL(<< "We should never come here");
        return std::strong_ordering::equal;
    }

public:
    const std::string& hand() const { return hand_; }
    int bid() const { return bid_; }

protected:
    static const int FIVE_OF_A_KIND = 6;
    static const int FOUR_OF_A_KIND = 5;
    static const int FULL_HOUSE = 4;
    static const int THREE_OF_A_KIND = 3;
    static const int TWO_PAIRS = 2;
    static const int ONE_PAIR = 1;
    static const int HIGH_CARD = 0;

    virtual int hand_strength() const {
        std::unordered_map<char, int> kinds;
        for (char ch : hand_) {
            ++kinds[ch];
        }

        return calculate_strength(kinds);
    }

    static int calculate_strength(const std::unordered_map<char, int>& kinds) {
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

        for (const auto& [key, value] : kinds) {
            if (value == 3) {
                return THREE_OF_A_KIND;
            }
        }

        return TWO_PAIRS;
    }

    size_t card_strength(char ch) const {
        const auto& cards = get_cards_array();

        auto it = std::ranges::find(cards, ch);
        VERIFY(it != cards.end(), << "Card doesn't exist");

        return std::distance(cards.begin(), it);
    }

    virtual const std::array<char, 13>& get_cards_array() const {
        static const std::array<char, 13> cards = {'2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A'};
        return cards;
    }

protected:
    std::string hand_;
    int bid_;
};

class HandNewAlgo : public Hand {
public:
    HandNewAlgo() = default;
    HandNewAlgo(const std::string& hand, int bid) : Hand(hand, bid) {}
    HandNewAlgo& operator=(const HandNewAlgo& hand) = default;

private:
    virtual const std::array<char, 13>& get_cards_array() const override {
        static const std::array<char, 13> cards = {'J', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'Q', 'K', 'A'};
        return cards;
    }

    virtual int hand_strength() const override {
        if (std::ranges::find(hand_, 'J') == hand_.end()) {
            return Hand::hand_strength();
        }

        std::unordered_map<char, int> kinds;
        for (char ch : hand_) {
            ++kinds[ch];
        }

        const size_t size = kinds.size();
        if (size == 1) {
            return FIVE_OF_A_KIND;
        }

        update_kinds(kinds);

        return calculate_strength(kinds);
    }

    static void update_kinds(std::unordered_map<char, int>& kinds) {
        size_t joker_count = 0;
        size_t max_card_count = 0;
        for (const auto& [key, value] : kinds) {
            if (key == 'J') {
                joker_count = value;
                continue;
            }

            if (value > max_card_count) {
                max_card_count = value;
            }
        }

        kinds.erase('J');
        for (auto& [key, value] : kinds) {
            if (value == max_card_count) {
                value += joker_count;
                break;
            }
        }
    }
};


} // namespace

namespace task7 {

int main() {
    // typedef Hand HandType;
    typedef HandNewAlgo HandType;

    std::ifstream input("7.input");
    VERIFY(input, << "Can't open file");

    std::vector<HandType> hands; 
    std::string line;
    while (std::getline(input, line)) {
        auto [hand, bid] = parse_line(line);
        std::cout << "Hand is " << hand << ", bid is " << bid << std::endl;

        hands.emplace_back(hand, bid);
    }

    std::ranges::sort(hands, std::less());

    unsigned long result = 0;
    for (int i = 0; i < hands.size(); ++i) {
        const auto& hand = hands[i];
        std::cout << "Hand " << hand.hand() << " has rank " << i + 1 << " and bid = " << hand.bid() << std::endl;
        result += (i + 1) * hand.bid(); 
    }
    
    std::cout << "Result is " << result << std::endl;
    return 0;
}


} // task7