#include "Expense.h"
#include <iostream>
#include <iomanip>

Expense::Expense(double m, Date d, std::string cat, std::string curr, std::string part)
    : Transaction(m, d, cat, curr, part) {}

Expense::~Expense() {
}

double Expense::realValue() const {
    return -money;                                                                                                      //az emlitett modon negativ erteket ad vissza
}

void Expense::print() const {
    std::cout << "[EXPENSE]  -" << money << " " << currency
        << "\t| Date: " << date.year << "." << date.month << "." << date.day << ". " << std::setfill('0') << std::setw(2) << date.hour << ":" << std::setw(2) << date.minute << std::setfill(' ')
        << "\t| Cat: " << category
        << "\t| Partner: " << partner << std::endl;
}

void Expense::saveToFile(std::ostream& out) const {
    out << "K " << money << " "
        << date.year << " " << date.month << " " << date.day << " "
        << date.hour << " " << date.minute << " "
        << category << " " << currency << " " << partner << "\n";
}