#ifndef JOURNAL_H
#define JOURNAL_H
#include <string>
#include <vector>
#include "transaction.h"

class Journal {
    std::string filename;
public:
    Journal(std::string f);
    void append(Transaction t);
    std::vector<Transaction> readAll();
};
#endif
