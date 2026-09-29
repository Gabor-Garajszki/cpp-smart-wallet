#pragma once
#include "Transaction.h"

class Expense : public Transaction {
public:
    Expense(double m, Date d, std::string cat, std::string curr, std::string part);

    ~Expense();

    double realValue() const override;

    void print() const override;

    void saveToFile(std::ostream& out) const override;
};