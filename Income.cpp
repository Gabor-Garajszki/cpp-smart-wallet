#include "Income.h"
#include <iostream>
#include <iomanip>

Income::Income(double m, Date d, std::string cat, std::string curr, std::string part)
    : Transaction(m, d, cat, curr, part) {}

Income::~Income() {
}

double Income::realValue() const {
    return money;
}

void Income::print() const {
    std::cout << "[INCOME]   +" << money << " " << currency
        << "\t| Date: " << date.year << "." << date.month << "." << date.day << ". " << std::setfill('0') << std::setw(2) << date.hour << ":" << std::setw(2) << date.minute << std::setfill(' ')
        << "\t| Cat: " << category
        << "\t| Partner: " << partner << std::endl;
}

void Income::saveToFile(std::ostream& out) const {
    out << "B " << money << " "
        << date.year << " " << date.month << " " << date.day << " "
        << date.hour << " " << date.minute << " "
        << category << " " << currency << " " << partner << "\n";
}