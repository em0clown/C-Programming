#ifndef ACCOUNT_H
#define ACCOUNT_H
#include <vector>
#include "transaction.h"

class Account {
    double balance;
    double dailyLimitUsed;
public:
    Account();
    void deposit(double amt);
    bool withdraw(double amt);
    double getBalance() const;
    void setBalance(double b);
    double getDailyUsed() const;
};
#endif
