#include "account.h"
#include <algorithm>
Account::Account() : balance(0), dailyLimitUsed(0) {}
void Account::deposit(double amt) { balance += amt; }
bool Account::withdraw(double amt) {
    double fee = std::max(30.0, amt * 0.01);
    if (balance >= (amt + fee)) {
        balance -= (amt + fee);
        dailyLimitUsed += amt;
        return true;
    }
    return false;
}
double Account::getBalance() const { return balance; }
void Account::setBalance(double b) { balance = b; }
double Account::getDailyUsed() const { return dailyLimitUsed; }
