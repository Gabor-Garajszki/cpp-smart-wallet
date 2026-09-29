#pragma once
#include <string>
#include <ostream>

struct Date {                                                                           //Egy datum struct
    int year;
    int month;
    int day;
    int hour;
    int minute;
};

class Transaction {                                                                     //maga a class ami elmenti a tranzakcio fontos adatait: az osszeget, a datumot, a kategoriajat a penznemet es hogy kitol/kinek lett utalva

protected:
    double money;
    Date date;
    std::string category;
    std::string currency;
    std::string partner;

public:                                                                                 //a fuggvenyei: kon es destruktor + a kiadasok miatti fuggveny ami negativva teszi az osszeget
    Transaction(double m, Date d, std::string cat, std::string curr, std::string part);

    virtual ~Transaction();

    Transaction(const Transaction&) = delete;

    Transaction& operator=(const Transaction&) = delete;

    virtual double realValue() const = 0;

    virtual void print() const = 0;

    Date getDate() const { return date; }
    virtual void saveToFile(std::ostream& out) const = 0;

    std::string getCategory() const { return category; }

    std::string getPartner() const { return partner; }

    std::string getCurrency() const { return currency; }
};