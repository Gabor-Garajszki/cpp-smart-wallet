#include <iostream>                                                                                                     //a szukseges dolgok meghivasa
#include <exception>
#include "Wallet.h"
#include "Income.h"
#include "Expense.h"
#include <limits>
#include <cctype>
#include <cstdlib>
                                                                                                                        //ezek a fuggvenyek lesznek a mainben is felhasznalva
bool authenticate(Wallet& myWallet);                                                                                    //jelszoellenorzo
int getValidInt(const std::string& prompt = " ");                                                                       //csak intet enged befogadni, kulonben ujrakeri az erteket
double getValidDouble(const std::string& prompt = " ");                                                                 //ugyan az csak doubleval
Date getValidDate(const std::string& prompt = " ");                                                                     //same csak dateval
void addNewTransaction(Wallet& myWallet, int& save, const int choice);                                                  //valasztastol fuggoen egy bevetelt vagy kiadast felvezet
void handleModifyOrDelete(Wallet& myWallet, int& save);                                                                 //elvegzi egy tranzakcio modositasat vagy torleset
std::string getValidCurrencyCode(const std::string& prompt = "");                                                       //ugyan     csak           inputokat          megadni
std::string getValidString(const std::string& prompt = " ");                                                            //      ugy      megfelelo           engednek
void handleCurrencyChange(Wallet& myWallet, int& save);                                                                 //egy valotanak valtoztathato lesz a forinthoz viszonyitott arfolyama
void printMainMenu();                                                                                                   //csak hogy rovidebb legyen a main
void printMenu3();
void passwordChangerOption(Wallet& myWallet, int& save);
void saveOption(Wallet& myWallet, int& save, int& run, const std::string filename);

int main() {

    Wallet myWallet;                                                                                                    //Wallet object letrefozasa

    std::string filename;                                                                                               //A több felhasznalo miatt a filenev is be lessz olvasva

    std::cout << "--- Welcome to Smart Wallet ---" << std::endl;
    std::cout << "Enter the data file name you want to use (or press Enter for default 'adatok.txt'): ";

    std::getline(std::cin, filename);                                                                                   //Itt meg is tortenik a beolvasas

    if (filename.empty()) {                                                                                             //ami ha ures akkor a default adatok.txt lesz
        filename = "adatok.txt";
    }

    try {                                                                                                               //elindul a betoltes, ha sikeresen le is zajlik akkor azt ki is irja
        myWallet.loadFromFile(filename);
        std::cout << "\nData successfully loaded from " << filename << "!" << std::endl;
    }
    catch (const std::exception& e) {                                                                                   //De ha valami hiba tortenik akkor ez fut le
        std::cout << "\nNotice: " << e.what() << std::endl;
        std::cout << "Starting with an empty wallet. A new file will be created upon saving." << std::endl;
    }

    if (myWallet.getPassword().empty()) {                                                                               //Ha ures a file akkor jelszo sem lesz. ezt itt fogjuk letrehozni
        std::string newPwd;
        std::cout << "\nIt seems this is a new Wallet. Please set a password: ";
        newPwd = getValidString();
        myWallet.setPassword(newPwd);
        std::cout << "Password successfully set!" << std::endl;
    }
    else {                                                                                                              //ha volt jelszo akkor az itt lesz ellenorizve. 3 probalkozas van kitalalni, ha nem jon ossze akkor bezar a program
        if (!authenticate(myWallet)) {
            std::cout << "\nToo many failed attempts. Security lock initiated. Exiting program..." << std::endl;
            return 1;
        }
    }

    std::cout << "\nYour Smart Wallet is ready to use! Choose your option:" << std::endl;


    int run = 1;                                                                                                        //amig a run erteke 1 addig megy a while ciklus
    int choice;                                                                                                         //elmenti hogy mit akar csinalni a felhasznalo
    int save = 1;                                                                                                       //megjegyzi hogy van e friss mentes, kilepeskor hasznalatos

    while (run == 1) {
        printMainMenu();

        choice = getValidInt("Your choice: ");

        switch (choice) {
            case 1: {
                addNewTransaction(myWallet, save, choice);
                save = 0;
                break;
            }

            case 2: {
                addNewTransaction(myWallet, save, choice);
                save = 0;
                break;
            }                                                                                                           //semmi extra idaig, csak az opcio kivalasztasa es a tranzakcio hozzaadasa van eddig

            case 3:
                printMenu3();
            
                choice = getValidInt("Your choice: ");

                myWallet.sortByDate();                                                                                  //idorendi sorrendbe rendezi a tranzakciokat

                switch (choice) {
                    case 1:
                        myWallet.printAll();
                        break;

                    case 2: {
                        std::cout << "\n--- Search Between Dates ---" << std::endl;

                        Date startDate, endDate;

                        startDate = getValidDate("Enter the START date (Year Month Day Hour Minute - separated by spaces): ");

                        endDate = getValidDate("Enter the END date (Year Month Day Hour Minute - separated by spaces): ");

                        myWallet.printBetweenDates(startDate, endDate);
                        break;
                    }

                    case 3: {
                        std::string searchCat;
                        std::cout << "\n--- Search by Category ---" << std::endl;
                        searchCat = getValidString("Enter the category you want to search for: ");

                        myWallet.printByCategory(searchCat);
                        break;
                    }

                    case 4: {
                        std::string searchPartner;
                        std::cout << "\n--- Search by Partner ---" << std::endl;
                        searchPartner = getValidString("Enter the partner's name you want to search for: ");

                        myWallet.printByPartner(searchPartner);
                        break;
                    }                                                                                                   //ertelemszeruen a a kategorianak megfelelo fuggvenyek vannak meghivva miutan be lettek kerve a szukseges adatok

                    default:
                        std::cout << "Invalid choice, returning to main menu." << std::endl;
                        break;
                }

                break;

            case 4:
                handleModifyOrDelete(myWallet, save);
                break;

            case 5:
                handleCurrencyChange(myWallet, save);
                break;

            case 6: {
                passwordChangerOption(myWallet, save);
                break;
            }

            case 7:                                                                                                     //mentesi lehetoseg
                std::cout << "\n--- Saving Data ---" << std::endl;

                myWallet.saveToFile(filename);

                save = 1;
                break;

            case 8:
                saveOption(myWallet, save, run, filename);
                break;

            default:
                std::cout << "Your choice was wrong, try again" << std::endl;
                break;
        }
    }

    return 0;
}

bool authenticate(Wallet& myWallet) {
    std::string inputPwd;
    int attempts = 3;

    while (attempts > 0) {
        std::cout << "\nEnter password (" << attempts << " attempts remaining): ";
        inputPwd = getValidString();

        if (myWallet.checkPassword(inputPwd)) {
            std::cout << "Correct password!" << std::endl;
            return true;
        }
        else {
            std::cout << "Access Denied. Incorrect password." << std::endl;
            attempts--;
        }
    }
    return false;
}

int getValidInt(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && value >= 0) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            break;
        }
        else {
            std::cout << "Error: Invalid input! Please enter a number." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
    return value;
}

double getValidDouble(const std::string& prompt) {
    double value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && value >= 0) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            break;
        }
        else {
            std::cout << "Error: Invalid input! Please enter a number." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
    return value;
}

Date getValidDate(const std::string& prompt) {
    Date tempDate;
    while (true) {
        std::cout << prompt;
        if (std::cin >> tempDate.year >> tempDate.month >> tempDate.day >> tempDate.hour >> tempDate.minute) {

            if (tempDate.year < 1900 ||
                tempDate.month < 1 || tempDate.month > 12 ||
                tempDate.day < 1 || tempDate.day > 31 ||
                tempDate.hour < 0 || tempDate.hour > 23 ||
                tempDate.minute < 0 || tempDate.minute > 59) {
                std::cout << "Error: Invalid date logic! Please enter a real date/time and dont be this much funny hahah." << std::endl;
                continue;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            break;
        }
        else {
            std::cout << "Error: Invalid input! Please enter exactly 5 numbers separated by spaces." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
    return tempDate;
}

void addNewTransaction(Wallet& myWallet, int& save, const int choice) {                                                 //siman bekeri az ertekeket es elmenti a tombbe
    std::cout << "\n--- Add New Transaction ---" << std::endl;
    double amount;
    std::string category, currency, partner;

    std::cout << "Amount (positive number): ";
    amount = getValidDouble();

    Date d;
    std::cout << "Date (Year Month Day Hour Minute - separated by spaces, e.g., 1999 12 31 23 59): ";
    d = getValidDate();

    std::cout << "Category (e.g., Salary, one word): ";
    category = getValidString();

    std::cout << "Currency (e.g., HUF): ";
    currency = getValidCurrencyCode();

    if (!myWallet.hasCurrency(currency)) {                                                                              //ha meg nem hasznalt valutat vitt fel akkor a mi listankhoz is hozza kell adnia az erteket
        std::cout << "Notice: The currency '" << currency << "' is new to the Wallet." << std::endl;
        std::cout << "Please enter its exchange rate to HUF: ";
        double rate = getValidDouble();

        while (rate <= 0) {
            std::cout << "Error: The exchange rate must be strictly positive! Try again: ";
            rate = getValidDouble();
        }

        myWallet.setCurrencyRate(currency, rate);
    }

    std::cout << "Partner (e.g., Workplace, one word): ";

    partner = getValidString();

    if (choice == 1) {
        Transaction* newIncome = new Income(amount, d, category, currency, partner);
        myWallet.addTransaction(newIncome);
    }
    else if (choice == 2) {
        Transaction* newExpense = new Expense(amount, d, category, currency, partner);
        myWallet.addTransaction(newExpense);
    }

    std::cout << "Successfully added to the wallet!" << std::endl;
    save = 0;
}

void handleModifyOrDelete(Wallet& myWallet, int& save) {
    std::cout << "\n--- Modify or Delete Transaction ---" << std::endl;

    myWallet.sortByDate();
    myWallet.printWithIndices();                                                                                        //az egyszeru kivalasztasert idorendi sorrendben ki lesznek printelve a tranzakciok amibol egyszeruen kivalaszthatja a mudositani vagy torolni kivant tranzakciot

    int id;
    std::cout << "\nEnter the ID of the transaction you want to edit/delete (or 0 to cancel): ";
    id = getValidInt();

    if (id <= 0) {
        std::cout << "Operation cancelled." << std::endl;
        return;
    }

    while (id > myWallet.getCount()) {
        std::cout << "Wrong id, try again!" << std::endl;
        id = getValidInt();
        if (id <= 0) {
            std::cout << "Operation cancelled." << std::endl;
            return;
        }
    }

    int arrayIndex = id - 1;

    int action;
    std::cout << "What do you want to do with this transaction?\n";
    std::cout << "1 - Modify (Overwrite with new)\n";
    std::cout << "2 - Delete\n";
    std::cout << "Your choice: ";
    action = getValidInt();

    if (action == 2) {
        if (myWallet.deleteTransaction(arrayIndex)) {
            std::cout << "Transaction successfully deleted!" << std::endl;
            save = 0;
        }
        else {
            std::cout << "Invalid ID. Deletion failed." << std::endl;
        }
    }
    else if (action == 1) {
        std::cout << "\n--- Enter the NEW details for this transaction ---" << std::endl;

        int typeChoice;
        while (true) {
            std::cout << "Is the new transaction an Income (1) or Expense (2)? ";
            typeChoice = getValidInt("");
            if (typeChoice == 1 || typeChoice == 2) break;
            std::cout << "Error: Please enter 1 or 2!" << std::endl;
        }

        double amount;
        Date d;
        std::string category, currency, partner;

        std::cout << "Amount: ";
        amount = getValidDouble();
        std::cout << "Date (Year Month Day Hour Minute): ";
        d = getValidDate();
        std::cout << "Category: ";
        category = getValidString();
        std::cout << "Currency: ";
        currency = getValidCurrencyCode();
        if (!myWallet.hasCurrency(currency)) {
            std::cout << "Notice: The currency '" << currency << "' is new to the Wallet." << std::endl;
            std::cout << "Please enter its exchange rate to HUF: ";
            double rate = getValidDouble();
            myWallet.setCurrencyRate(currency, rate);
        }
        std::cout << "Partner: ";
        partner = getValidString();

        Transaction* replacement = nullptr;

        if (typeChoice == 1) {
            replacement = new Income(amount, d, category, currency, partner);
        }
        else {
            replacement = new Expense(amount, d, category, currency, partner);
        }

        if (myWallet.modifyTransaction(arrayIndex, replacement)) {
            std::cout << "Transaction successfully modified!" << std::endl;
            save = 0;
        }
        else {
            std::cout << "Invalid ID. Modification failed." << std::endl;
            delete replacement;
        }
    }
    else {
        std::cout << "Invalid action choice." << std::endl;
    }
}

std::string getValidCurrencyCode(const std::string& prompt) {
    std::string curr;
    while (true) {
        std::cout << prompt;
        std::cin >> curr;

        if (curr.length() != 3) {
            std::cout << "Error: The currency code must be exactly 3 characters long (e.g., EUR, USD)!" << std::endl;
            continue;
        }

        bool allUpper = true;
        for (char c : curr) {
            if (!std::isupper(c)) {
                allUpper = false;
                break;
            }
        }

        if (!allUpper) {
            std::cout << "Error: The currency code must contain only uppercase letters (A-Z)!" << std::endl;
            continue;
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        break;
    }
    return curr;
}

std::string getValidString(const std::string& prompt) {
    std::string input;
    while (true) {
        std::cout << prompt;
        std::cin >> std::ws;
        std::getline(std::cin, input);

        if (input.find(' ') != std::string::npos) {
            std::cout << "Error: Spaces are not allowed! Please use a single word (e.g., Fast_Food)." << std::endl;
        }
        else {
            break;
        }
    }
    return input;
}

void handleCurrencyChange(Wallet& myWallet, int& save) {
    std::cout << "\n--- Change/Add Currency Rate ---" << std::endl;
    myWallet.printCurrencies();

    std::string currName;
    double currRate;

    currName = getValidCurrencyCode("\nEnter the currency code (e.g., EUR, USD, HUF): ");

    std::cout << "Enter the new exchange rate: ";
    currRate = getValidDouble();

    myWallet.setCurrencyRate(currName, currRate);
    std::cout << "Currency '" << currName << "' successfully updated to " << currRate << "!" << std::endl;
    save = 0;
}

void printMainMenu() {
    std::cout << "\n--- MAIN MENU ---" << std::endl;
    std::cout << "Press number 1 to add a new income" << std::endl;
    std::cout << "Press number 2 to add a new expense" << std::endl;
    std::cout << "Press number 3 to see your Wallet's statistic" << std::endl;
    std::cout << "Press number 4 to modify or delete a previous transaction" << std::endl;
    std::cout << "Press number 5 to change a currency's spot value" << std::endl;
    std::cout << "Press number 6 to change your password" << std::endl;
    std::cout << "Press number 7 to save the datas" << std::endl;
    std::cout << "Press number 8 to exit" << std::endl;
}

void printMenu3() {
    std::cout << "Press number 1 to show the overall statistic" << std::endl;
    std::cout << "Press number 2 to show the transactions between two specific date" << std::endl;
    std::cout << "Press number 3 to show the transactions of a certain category" << std::endl;
    std::cout << "Press number 4 to show the transactions between You and a specific person" << std::endl;
}

void passwordChangerOption(Wallet& myWallet, int& save) {
    std::cout << "\n--- Change Master Password ---" << std::endl;
    std::cout << "To change your password, you must first verify your identity." << std::endl;

    if (authenticate(myWallet)) {
        std::string newPwd;
        newPwd = getValidString("\nEnter your NEW password: ");

        myWallet.setPassword(newPwd);
        std::cout << "Password successfully updated!" << std::endl;
        save = 0;
    }
    else {
        std::cout << "\nToo many failed attempts. Security lock initiated. Exiting program..." << std::endl;//ha rossz a jelszo akkor kilep buntetesbol mentes nelkul a programbol
        exit(1);
    }
}

void saveOption(Wallet& myWallet, int& save, int& run, const std::string filename) {
    if (save == 0) {
        std::cout << "Are you sure you want to exit without saving?" << std::endl;
        save = getValidInt("Type 0 if yes or type 1 if you want to save before exit the program: ");

        while (!(save == 0 || save == 1)) {
            std::cout << "\nThere must been a typo. try again" << std::endl;
            save = getValidInt();
        }

        if (save == 1) {
            myWallet.saveToFile(filename);
        }
    }

    std::cout << "Exiting program. Goodbye!" << std::endl;
    run = 0;                                                                                                // Ez megszakítja a while ciklust
}