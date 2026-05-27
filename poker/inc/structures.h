#pragma once
#include <vector>
#include <string>

enum GameMode { MENU, SINGLE_BOT, MULTI_HOTSEAT };

struct Card {
    int rank;
    int suit;
};

struct HandRank {
    int rank;
    std::vector<int> kickers;
    std::string name;
};

struct Player {
    std::string name;
    int money = 1000;
    int roundBet = 0;
    std::vector<Card> hand;
    bool isFolded = false;
};