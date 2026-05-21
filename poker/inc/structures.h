#pragma once
#include <vector>
#include <string>

enum GameMode { MENU, SINGLE_BOT, MULTI_HOTSEAT };

struct Card {
    int rank;  // 2-14 (11=J, 12=Q, 13=K, 14=A)
    int suit;  // 0=♥, 1=♦, 2=♣, 3=♠
};

struct HandRank {
    int rank;  // 1-9
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