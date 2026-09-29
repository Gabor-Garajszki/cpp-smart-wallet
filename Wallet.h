#pragma once
#include "Transaction.h"
#include <map>
#include <string>
#include <fstream>

class Wallet {
private:
    Transaction** transactions;
    int capacity;
    int count;

    std::map<std::string, double> exchangeRates;

    std::string password;

    bool isLater(const Date& d1, const Date& d2) const;

    void getCurrencies(std::ifstream& file);

    std::string encodePassword(const std::string& pwd) const;
    std::string decodePassword(const std::string& hexPwd) const;

public:
    Wallet(int cap = 16);

    ~Wallet();

    Wallet(const Wallet&) = delete;

    Wallet& operator=(const Wallet&) = delete;

    void loadFromFile(const std::string& filename);

    void addTransaction(Transaction* t);

    std::string getPassword() const { return password; }

    void setPassword(const std::string& newPassword) { password = newPassword; }

    void sortByDate();

    void printAll() const;

    void printBetweenDates(const Date& start, const Date& end) const;

    void printByCategory(const std::string& targetCategory) const;

    void printByPartner(const std::string& targetPartner) const;

    bool deleteTransaction(int index);

    void printCurrencies() const;

    void saveToFile(const std::string& filename) const;

    void setCurrencyRate(const std::string& curr, double rate);

    void printWithIndices() const;

    bool modifyTransaction(int index, Transaction* newTrans);

    bool checkPassword(const std::string& p) const { return password == p; }

    bool hasCurrency(const std::string& curr) const { return exchangeRates.find(curr) != exchangeRates.end(); }

    int getCount() { return count; }
};