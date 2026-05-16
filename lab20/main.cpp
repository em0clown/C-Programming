#include <iostream>
#include "card.h"
#include "account.h"
#include "journal.h"
#include "recovery.h"
#include "atm_engine.h"

int main() {
    Card myCard("4444", "1234");
    Account myAcc;
    Journal myJrnl("journal.bin");

    Recovery::checkAndRestore(myAcc, myJrnl);

    std::string inputPin;
    int tries = 0;
    bool authenticated = false;

    while (tries < 3) {
        std::cout << "Enter PIN: ";
        std::cin >> inputPin;
        if (myCard.verifyPin(inputPin)) {
            authenticated = true;
            break;
        }
        if (myCard.isBlocked()) break;
        std::cout << "Wrong! Tries left: " << (2 - tries) << std::endl;
        tries++;
    }

    if (!authenticated) {
        std::cout << "Access denied. Card blocked or too many attempts." << std::endl;
        return 1;
    }

    ATMEngine engine(myAcc, myJrnl, myCard);
    engine.runMenu();

    return 0;
}
