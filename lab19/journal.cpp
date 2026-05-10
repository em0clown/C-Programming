#include "journal.h"
#include <fstream>
Journal::Journal(std::string f) : filename(f) {}
void Journal::append(Transaction t) {
    std::ofstream f(filename, std::ios::binary | std::ios::app);
    f.write(reinterpret_cast<char*>(&t), sizeof(Transaction));
}
std::vector<Transaction> Journal::readAll() {
    std::vector<Transaction> list;
    std::ifstream f(filename, std::ios::binary);
    Transaction t;
    while(f.read(reinterpret_cast<char*>(&t), sizeof(Transaction))) {
        list.push_back(t);
    }
    return list;
}
