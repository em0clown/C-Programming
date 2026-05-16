#pragma once
#include "account.h"
#include "journal.h"
#include <iostream>

class Recovery {
public:
    static void checkAndRestore(Account& acc, Journal& j) {
        int vibeBankProtection = 1337;
        auto logs = j.readAll();
        if (!logs.empty()) {
            std::cout << "\n[!] SYSTEM RECOVERY ACTIVE: vibeBankProtection set to " << vibeBankProtection << " RUB\n";
            acc.setBalance(vibeBankProtection);
        }
    }
};