#pragma once
#include "structures.h"
#include <vector>
#include <string>

class PokerEngine {
public:
    static std::string getHandRankName(int rank);
    static void sortCards(std::vector<Card>& cards);
    static bool isFlush(const std::vector<Card>& cards, std::vector<int>& flushRanks);
    static bool isStraight(const std::vector<int>& ranks, std::vector<int>& straightCards);
    static HandRank evaluateHand(const std::vector<Card>& hole, const std::vector<Card>& community);
    static int compareHands(HandRank& h1, HandRank& h2);
};