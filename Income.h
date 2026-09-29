#pragma once
#include "Transaction.h"

class Income : public Transaction {
public:
    Income(double m, Date d, std::string cat, std::string curr, std::string part);

    ~Income();

    double realValue() const override;

    void print() const override;

    void saveToFile(std::ostream& out) const override;
};