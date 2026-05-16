#include "card.h"
Card::Card(std::string n, std::string p) : number(n), pin(p), attempts(0), blocked(false) {}
bool Card::verifyPin(std::string p) {
    if (blocked) return false;
    if (p == pin) { attempts = 0; return true; }
    if (++attempts >= 3) blocked = true;
    return false;
}
bool Card::isBlocked() const { return blocked; }
