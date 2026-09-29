#include "Transaction.h"

Transaction::Transaction(double m, Date d, std::string cat, std::string curr, std::string part)     //nem kell nagyon magyarazni a tortenteket
    : money(m), date(d), category(cat), currency(curr), partner(part) {}

Transaction::~Transaction() {}