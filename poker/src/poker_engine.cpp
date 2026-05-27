#include "poker_engine.h"
#include <algorithm>
#include <map>

using namespace std;

string PokerEngine::getHandRankName(int rank) {
    static const vector<string> names = {
        "Ошибка", "Старшая карта", "Пара", "Две пары",
        "Сет", "Стрит", "Флеш", "Фулл-хаус", "Каре", "Стрит-флеш"
    };
    return (rank >= 1 && rank <= 9) ? names[rank] : "Неизвестно";
}

void PokerEngine::sortCards(vector<Card>& cards) {
    sort(cards.begin(), cards.end(), [](const Card& a, const Card& b) {
        return a.rank > b.rank;
    });
}

bool PokerEngine::isFlush(const vector<Card>& cards, vector<int>& flushRanks) {
    int suitCount[4] = {0, 0, 0, 0};
    for (const auto& c : cards) {
        suitCount[c.suit]++;
    }
    for (int s = 0; s < 4; s++) {
        if (suitCount[s] >= 5) {
            for (const auto& c : cards) {
                if (c.suit == s) {
                    flushRanks.push_back(c.rank);
                }
            }
            sort(flushRanks.begin(), flushRanks.end(), greater<int>());
            return true;
        }
    }
    return false;
}

bool PokerEngine::isStraight(const vector<int>& ranks, vector<int>& straightCards) {
    vector<int> uniqueRanks = ranks;
    sort(uniqueRanks.begin(), uniqueRanks.end(), greater<int>());
    uniqueRanks.erase(unique(uniqueRanks.begin(), uniqueRanks.end()), uniqueRanks.end());

    if (uniqueRanks.size() < 5) return false;
    for (size_t i = 0; i + 4 < uniqueRanks.size(); i++) {
        if (uniqueRanks[i] == uniqueRanks[i+4] + 4) {
            straightCards.push_back(uniqueRanks[i]);
            return true;
        }
    }
    if (count(uniqueRanks.begin(), uniqueRanks.end(), 14) &&
        count(uniqueRanks.begin(), uniqueRanks.end(), 2) &&
        count(uniqueRanks.begin(), uniqueRanks.end(), 3) &&
        count(uniqueRanks.begin(), uniqueRanks.end(), 4) &&
        count(uniqueRanks.begin(), uniqueRanks.end(), 5)) {
        straightCards.push_back(5);
        return true;
    }
    return false;
}

HandRank PokerEngine::evaluateHand(const vector<Card>& hole, const vector<Card>& community) {
    vector<Card> allCards = hole;
    allCards.insert(allCards.end(), community.begin(), community.end());
    sortCards(allCards);

    HandRank result{1, {}, "Старшая карта"};
    map<int, int> rankCount;
    vector<int> ranks;

    for (const auto& c : allCards) {
        rankCount[c.rank]++;
        ranks.push_back(c.rank);
    }

    vector<pair<int, int>> freq(rankCount.begin(), rankCount.end());
    sort(freq.begin(), freq.end(), [](const pair<int, int>& a, const pair<int, int>& b) {
        if (a.second != b.second) return a.second > b.second;
        return a.first > b.first;
    });

    vector<int> flushRanks;
    bool flush = isFlush(allCards, flushRanks);
    vector<int> straightRanks;
    bool straight = isStraight(ranks, straightRanks);

    if (flush && straight) {
        result.rank = 9;
        result.name = "Стрит-флеш";
        result.kickers = straightRanks;
        return result;
    }
    if (freq[0].second == 4) {
        result.rank = 8;
        result.name = "Каре";
        result.kickers.push_back(freq[0].first);
        result.kickers.push_back(freq[1].first);
        return result;
    }
    if (freq[0].second == 3 && freq.size() > 1 && freq[1].second >= 2) {
        result.rank = 7;
        result.name = "Фулл-хаус";
        result.kickers.push_back(freq[0].first);
        result.kickers.push_back(freq[1].first);
        return result;
    }
    if (flush) {
        result.rank = 6;
        result.name = "Флеш";
        result.kickers = flushRanks;
        return result;
    }
    if (straight) {
        result.rank = 5;
        result.name = "Стрит";
        result.kickers = straightRanks;
        return result;
    }
    if (freq[0].second == 3) {
        result.rank = 4;
        result.name = "Сет";
        result.kickers.push_back(freq[0].first);
        for (size_t i = 1; i < freq.size(); i++) {
            result.kickers.push_back(freq[i].first);
        }
        return result;
    }
    if (freq[0].second == 2 && freq.size() > 1 && freq[1].second == 2) {
        result.rank = 3;
        result.name = "Две пары";
        result.kickers.push_back(freq[0].first);
        result.kickers.push_back(freq[1].first);
        for (size_t i = 2; i < freq.size(); i++) {
            result.kickers.push_back(freq[i].first);
        }
        return result;
    }
    if (freq[0].second == 2) {
        result.rank = 2;
        result.name = "Пара";
        result.kickers.push_back(freq[0].first);
        for (size_t i = 1; i < freq.size(); i++) {
            result.kickers.push_back(freq[i].first);
        }
        return result;
    }
    result.rank = 1;
    result.name = "Старшая карта";
    for (size_t i = 0; i < allCards.size() && i < 5; i++) {
        result.kickers.push_back(allCards[i].rank);
    }
    return result;
}

int PokerEngine::compareHands(HandRank& h1, HandRank& h2) {
    if (h1.rank != h2.rank) {
        return h1.rank > h2.rank ? 1 : -1;
    }
    for (size_t i = 0; i < h1.kickers.size() && i < h2.kickers.size(); i++) {
        if (h1.kickers[i] != h2.kickers[i]) {
            return h1.kickers[i] > h2.kickers[i] ? 1 : -1;
        }
    }
    return 0;
}