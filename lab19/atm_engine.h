#ifndef ATM_ENGINE_H
#define ATM_ENGINE_H
#include "account.h"
#include "journal.h"
#include "card.h"
#include <iostream>
#include <ctime>

class ATMEngine {
    Account& acc;
    Journal& jrnl;
    Card& card;
public:
    ATMEngine(Account& a, Journal& j, Card& c) : acc(a), jrnl(j), card(c) {}

    void runMenu() {
        int choice;
        while (true) {
            std::cout << "\n=== VIBE BANK ATM ===\n";
            std::cout << "1. Check Balance\n";
            std::cout << "2. Deposit Money\n";
            std::cout << "3. Withdraw Money\n";
            std::cout << "4. Operation History\n";
            std::cout << "5. Exit\n";
            std::cout << "> ";
            
            if (!(std::cin >> choice)) {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                continue;
            }

            if (choice == 5) break;
            
            double amt;
            switch(choice) {
                case 1:
                    std::cout << "Current Balance: " << acc.getBalance() << " RUB\n";
                    break;
                case 2: 
                    std::cout << "Enter amount to deposit: ";
                    std::cin >> amt;
                    if (amt > 0) {
                        acc.deposit(amt);
                        jrnl.append({TransactionType::DEPOSIT, amt, std::time(nullptr)});
                        std::cout << "Success!\n";
                    }
                    break;
                case 3:
                    std::cout << "Enter amount to withdraw (max 10,000): ";
                    std::cin >> amt;
                    if (amt > 10000) {
                        std::cout << "Error: Max withdrawal limit is 10,000 RUB\n";
                    } else if (acc.getDailyUsed() + amt > 50000) {
                        std::cout << "Error: Daily limit (50,000) exceeded!\n";
                    } else if (acc.withdraw(amt)) {
                        jrnl.append({TransactionType::WITHDRAWAL, amt, std::time(nullptr)});
                        std::cout << "Success! (Fee applied: min 30 RUB or 1%)\n";
                    } else {
                        std::cout << "Error: Insufficient funds.\n";
                    }
                    break;
                case 4:
                {
                    auto logs = jrnl.readAll();
                    std::cout << "\n--- Last 5 Operations ---\n";
                    int start = (logs.size() > 5) ? (logs.size() - 5) : 0;
                    for (size_t i = start; i < logs.size(); i++) {
                        std::cout << (logs[i].type == TransactionType::DEPOSIT ? "[+]" : "[-]") 
                                  << " " << logs[i].amount << " RUB\n";
                    }
                    break;
                }
                default:
                    std::cout << "Invalid choice.\n";
            }
        }
    }
};
#endif
