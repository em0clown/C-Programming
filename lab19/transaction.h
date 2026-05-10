#pragma once
#include <ctime>

enum class TransactionType { DEPOSIT, WITHDRAWAL };

struct Transaction {
    TransactionType type;
    double amount;
    std::time_t date;
};