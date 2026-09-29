# Smart Wallet & Transaction Manager (C++)

A modular, object-oriented console application written in C++ for tracking and analyzing personal finances, multi-currency transactions, and category distributions.

## Key Features

- **Polymorphic Transactions:** Abstract base class (`Transaction`) with concrete implementations (`Income`, `Expense`) managing cash flow logic and terminal formatting.
- **Custom Memory Management:** Dynamic array container (`Wallet`) with geometric capacity scaling ($O(1)$ amortized insertion), RAII ownership semantics, and explicit Rule of Three/Five implementation (`copy constructor` and `assignment operator` deleted to prevent shallow copies).
- **Multi-Currency Support:** Runtime conversion to HUF via internal exchange rate tracking (`std::map`).
- **File Persistence & Basic Obfuscation:** Data serialization with file stream validation and XOR-based credential protection.
- **Robust Console I/O:** Stream state validation and buffer flushing against invalid user inputs.

## Project Structure

```text
├── Transaction.h / .cpp    # Abstract base class for transaction entities
├── Income.h / .cpp         # Concrete class for positive cash flows
├── Expense.h / .cpp        # Concrete class for expenses (negative valuation)
├── Wallet.h / .cpp         # Core container managing memory, rates, and sorting
└── SmartWallet.cpp         # Application entry point, CLI loop, and user validation

## Future Improvements
- Multi-threading support for concurrent transaction parsing.