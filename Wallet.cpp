#include "Wallet.h"
#include "Income.h"
#include "Expense.h"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <algorithm>

Wallet::Wallet(int cap) {                                                                       //KONSTRUKTOR: a jelenlegi mainben fix hogy 16 lesz a capacity, de a jövőtállóság miatt egy random erteket is felkerekit ketto hatvanyara
    capacity = 16;
    cap = std::min(cap, 1048576);
    while (capacity < cap) {
        capacity *= 2;
    }
    count = 0;

    transactions = new Transaction * [capacity];

    for (int i = 0; i < capacity; ++i) {
        transactions[i] = nullptr;
    }

    exchangeRates["HUF"] = 1.0;

    password = "";
}

Wallet::~Wallet() {                                                                             //DESTRUKTOR: szepen felszabaditja a memoriat
    for (int i = 0; i < count; ++i) {
        delete transactions[i];
    }

    delete[] transactions;
}

void Wallet::loadFromFile(const std::string& filename) {                                        //A main elejen ez fogja beolvasni a filebol az adatokat
    std::ifstream file(filename);

    if (!file.is_open()) {                                                                      //biztonsagi ellenorzes
        throw std::runtime_error("Error: the file not found or not openable: " + filename);
    }

    getCurrencies(file);                                                                        //Ez olvassa be a jelszot es a valutak ertekeit az elso sorbol. utana mar csak a tranzakciok maradnak

    std::string tipus;
    double m;
    int ev, ho, nap, ora, perc;
    std::string cat, curr, part;

    while (file >> tipus >> m >> ev >> ho >> nap >> ora >> perc >> cat >> curr >> part) {       //amig be tudja olvasni a dolgokat addig a kategoriajanak megfeleloen letrehozza a uj objectet
        Date d = { ev, ho, nap, ora, perc };
        Transaction* t = nullptr;

        if (tipus == "B") {
            t = new Income(m, d, cat, curr, part);
        }
        else if (tipus == "K") {
            t = new Expense(m, d, cat, curr, part);
        }

        if (t != nullptr) {                                                                     //es ha ez sikeresen meg is tortent akkor el is menti a tombbe
            addTransaction(t);
        }
    }

    file.close();
    std::cout << "The data was successfully loaded." << std::endl;
}

void Wallet::addTransaction(Transaction* t) {                                                   //Hozzaad meg egy tranzakciot a dinamikus tombhoz. mind a beolvasasnal es a kesobbi tranzakcio felvitelnel is hasznalatos ez a fuggveny
    if (count == capacity) {
        capacity *= 2;

        Transaction** temp = new Transaction * [capacity];

        for (int i = 0; i < count; ++i) {
            temp[i] = transactions[i];
        }

        for (int i = count; i < capacity; ++i) {
            temp[i] = nullptr;
        }

        delete[] transactions;

        transactions = temp;
    }

    transactions[count] = t;

    count++;
}

void Wallet::getCurrencies(std::ifstream& file) {                                               //Beolvassa a jelszot es a valutakat + ertekeit
    std::string firstLine;

    if (std::getline(file, firstLine)) {
        std::stringstream ss(firstLine);                                                        //Az egesz beolvasott elso sort atvaltja stringstreamra

        std::string tempPwd;
        ss >> tempPwd;
        password = decodePassword(tempPwd);

        std::string curr;
        double rate;

        while (ss >> curr >> rate) {                                                            //A valutask mennek az erre specialisan letrehozott tombbe
            exchangeRates[curr] = rate;
        }
    }
}

void Wallet::sortByDate() {                                                                     //egyszeru rendezo fuggveny. nem a leghatekonyabb de elvileg ilyen nagysagrendben nem lesz egyaltalan problema a futasi ideje
    for (int i = 0; i < count - 1; ++i) {
        for (int j = 0; j < count - i - 1; ++j) {
            Date date1 = transactions[j]->getDate();
            Date date2 = transactions[j + 1]->getDate();

            if (isLater(date1, date2)) {
                Transaction* temp = transactions[j];
                transactions[j] = transactions[j + 1];
                transactions[j + 1] = temp;
            }
        }
    }
}

bool Wallet::isLater(const Date& d1, const Date& d2) const {                                    //ennek a visszateresi erteke segit rendezni
    if (d1.year != d2.year) return d1.year > d2.year;
    if (d1.month != d2.month) return d1.month > d2.month;
    if (d1.day != d2.day) return d1.day > d2.day;
    if (d1.hour != d2.hour) return d1.hour > d2.hour;
    return d1.minute > d2.minute;
}

void Wallet::printAll() const {
    std::cout << "\n=== WALLET TRANSACTIONS (" << count << " items) ===" << std::endl;

    if (count == 0) {
        std::cout << "Your wallet is currently empty." << std::endl;
        std::cout << "=======================================" << std::endl;
        return;
    }

    double totalBalanceHUF = 0;
    std::map<std::string, double> categoryStats;                                                //ez a tomb az ossszes kategoriahoz szamolja a kiadasokat

    for (int i = 0; i < count; ++i) {
        transactions[i]->print();

        std::string curr = transactions[i]->getCurrency();
        double rate = 1.0;

        if (exchangeRates.find(curr) != exchangeRates.end()) {
            rate = exchangeRates.at(curr);
        }

        double hufValue = transactions[i]->realValue() * rate;
        totalBalanceHUF += hufValue;

        categoryStats[transactions[i]->getCategory()] += hufValue;
    }

    std::cout << "---------------------------------------" << std::endl;
    std::cout << "TOTAL BALANCE: " << totalBalanceHUF << " HUF" << std::endl;

    std::cout << "\n--- Net Balances by Category (HUF) ---" << std::endl;
    for (const auto& pair : categoryStats) {
        std::cout << pair.first << ": ";
        if (pair.second > 0) std::cout << "+";
        std::cout << pair.second << std::endl;
    }

    std::string topIncomeCat = "None";
    double maxIncome = 0.0;

    std::string topExpenseCat = "None";
    double maxExpense = 0.0;

    for (const auto& pair : categoryStats) {
        if (pair.second > maxIncome) {
            maxIncome = pair.second;
            topIncomeCat = pair.first;
        }
        if (pair.second < maxExpense) {
            maxExpense = pair.second;
            topExpenseCat = pair.first;
        }
    }

    std::cout << "\n--- Quick Insights ---" << std::endl;
    if (topIncomeCat != "None") {
        std::cout << "Top Income Category:  " << topIncomeCat << " (+" << maxIncome << ")" << std::endl;
    }
    if (topExpenseCat != "None") {
        std::cout << "Top Expense Category: " << topExpenseCat << " (" << maxExpense << ")" << std::endl;
    }

    std::cout << "=======================================" << std::endl;
}

bool Wallet::deleteTransaction(int index) {                                                     //kitorli a tranzakciot es elorebbtolja az egyel kesobb levoket, utana az utolsot fel is szabaditja
    if (index < 0 || index >= count) return false;

    delete transactions[index];

    for (int i = index; i < count - 1; ++i) {
        transactions[i] = transactions[i + 1];
    }

    transactions[count - 1] = nullptr;
    count--;

    return true;
}

void Wallet::printCurrencies() const {                                                          //valtoztatas elott megmutatja hogy mik vannak jelenleg
    std::cout << "--- Current Exchange Rates ---" << std::endl;
    
    if (exchangeRates.empty()) {
        std::cout << "No currencies recorded yet." << std::endl;
    }
    else {
        for (const auto& pair : exchangeRates) {
            std::cout << pair.first << " : " << pair.second << std::endl;
        }
    }
    std::cout << "------------------------------" << std::endl;
}

void Wallet::setCurrencyRate(const std::string& curr, double rate) {
    exchangeRates[curr] = rate;
}

void Wallet::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Error: cannot open file for saving: " + filename);
    }

    file << encodePassword(password) << " ";

    for (const auto& pair : exchangeRates) {
        file << pair.first << " " << pair.second << " ";
    }
    file << "\n";

    for (int i = 0; i < count; ++i) {
        transactions[i]->saveToFile(file);
    }

    file.close();
    std::cout << "The data has been successfully saved to " << filename << "!" << std::endl;
}

void Wallet::printByCategory(const std::string& targetCategory) const {
    std::cout << "\n=== TRANSACTIONS IN CATEGORY: " << targetCategory << " ===" << std::endl;

    bool found = false;

    for (int i = 0; i < count; ++i) {
        if (transactions[i]->getCategory() == targetCategory) {
            transactions[i]->print();
            found = true;
        }
    }

    // Ha egyetlen egyezés sem volt
    if (!found) {
        std::cout << "No transactions found in this category. Check if the Capital letters are used correctly" << std::endl;
    }
    std::cout << "=======================================" << std::endl;
}

void Wallet::printByPartner(const std::string& targetPartner) const {
    std::cout << "\n=== TRANSACTIONS WITH PARTNER: " << targetPartner << " ===" << std::endl;

    bool found = false;

    for (int i = 0; i < count; ++i) {
        if (transactions[i]->getPartner() == targetPartner) {
            transactions[i]->print();
            found = true;
        }
    }

    if (!found) {
        std::cout << "No transactions found with this partner." << std::endl;
    }

    std::cout << "==========================================" << std::endl;
}

void Wallet::printBetweenDates(const Date& start, const Date& end) const {
    std::cout << "\n=== TRANSACTIONS BETWEEN THE GIVEN DATES ===" << std::endl;

    bool found = false;

    for (int i = 0; i < count; ++i) {
        Date curr = transactions[i]->getDate();

        if (!isLater(start, curr) && !isLater(curr, end)) {
            transactions[i]->print();
            found = true;
        }
    }

    if (!found) {
        std::cout << "No transactions found in this period." << std::endl;
    }

    std::cout << "============================================" << std::endl;
}

void Wallet::printWithIndices() const {
    std::cout << "\n=== WALLET TRANSACTIONS ===" << std::endl;
    if (count == 0) {
        std::cout << "Your wallet is currently empty." << std::endl;
        return;
    }
    for (int i = 0; i < count; ++i) {
        std::cout << "ID: " << (i + 1) << " | ";
        transactions[i]->print();
    }
    std::cout << "===========================" << std::endl;
}

bool Wallet::modifyTransaction(int index, Transaction* newTrans) {
    if (index < 0 || index >= count) return false;

    delete transactions[index];
    transactions[index] = newTrans;

    return true;
}

std::string Wallet::encodePassword(const std::string& pwd) const {
    std::string result = "";
    const char hex_chars[] = "0123456789ABCDEF";
    for (unsigned char c : pwd) {
        c = c ^ 'G';
        result += hex_chars[c >> 4];
        result += hex_chars[c & 15];
    }
    return result;
}

std::string Wallet::decodePassword(const std::string& hexPwd) const {
    std::string result = "";
    for (size_t i = 0; i < hexPwd.length(); i += 2) {
        std::string byteString = hexPwd.substr(i, 2);
        char byte = (char)strtol(byteString.c_str(), NULL, 16);
        result += (byte ^ 'G');
    }
    return result;
}