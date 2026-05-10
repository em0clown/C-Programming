#ifndef CARD_H
#define CARD_H
#include <string>

class Card {
    std::string number;
    std::string pin;
    int attempts;
    bool blocked;
public:
    Card(std::string n, std::string p);
    bool verifyPin(std::string p);
    bool isBlocked() const;
};
#endif
