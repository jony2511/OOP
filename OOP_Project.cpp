#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>

using namespace std;

class Transaction
{
public:
    Transaction(const string &date, const string &description, double amount) : date(date), description(description), amount(amount) {}

    virtual double getAmount() const
    {
        return amount;
    }
    virtual string getType() const
    {
        return "Transaction";
    }

    string getDate() const
    {
        return date;
    }
    string getDescription() const
    {
        return description;
    }

    virtual void printDetails() const
    {
        cout << "Date: " << date << "\n";
        cout << "Description: " << description << "\n";
        cout << "Amount: $" << getAmount() << "\n";
    }

    // Overloading the less than operator to compare transactions based on amount
    bool operator<(const Transaction &other) const
    {
        return amount < other.amount;
    }

    // Friend function to overload the stream insertion operator (<<)
    friend ostream &operator<<(ostream &os, const Transaction &transaction)
    {
        os << "Date: " << transaction.date << "\n";
        os << "Description: " << transaction.description << "\n";
        os << "Amount: $" << transaction.amount << "\n";
        return os;
    }
    double amount;

private:
    string date;
    string description;
}; 

class Expense : public Transaction
{
public:
    Expense(const string &date, const string &description, double amount) : Transaction(date, description, amount) {}

    double getAmount() const override { return amount; }
    string getType() const override { return "Expense"; }

    void printDetails() const override
    {
        cout << "[Expense]\n";
        Transaction::printDetails();
    }
};

class Income : public Transaction
{
public:
    Income(const string &date, const string &description, double amount) : Transaction(date, description, amount) {}

    string getType() const override
    {
        return "Income";
    }
};

class ExpenseTracker
{
public:
    ExpenseTracker(const string &fileName) : fileName(fileName)
    {
        loadExpensesFromFile();
    }

    ~ExpenseTracker()
    {
    }

    void addTransaction(unique_ptr<Transaction> transaction)
    {
        transactions.push_back(move(transaction));
        saveExpensesToFile(); // Save immediately
    }

    void listTransactions() const
    {
        ifstream file(fileName);
        if (!file.is_open())
        {
            cerr << "Error: Unable to open the file for reading.\n";
            return;
        }

        string line;
        while (getline(file, line))
        {
            cout << line << endl;
        }

        file.close();
    }

    double calculateTotalExpenses() const
    {
        ifstream file(fileName);
        if (!file.is_open())
        {
            cerr << "Error: Unable to open the file for reading.\n";
            return 0.0;
        }

        double totalExpenses = 0.0;
        string line;
        while (getline(file, line))
        {
            if (line == "[Expense]")
            {
                double amount;
                file.ignore(1000, '$'); // Ignoring characters until '$' symbol
                file >> amount;
                totalExpenses += amount; // Calculating Total expense
            }
        }

        file.close();
        return totalExpenses;
    }

    double calculateTotalIncome() const
    {
        ifstream file(fileName);
        if (!file.is_open())
        {
            cerr << "Error: Unable to open the file for reading.\n";
            return 0.0;
        }

        double totalIncome = 0.0;
        string line;
        while (getline(file, line))
        {
            if (line == "[Income]")
            {
                double amount;
                file.ignore(1000, '$'); // Ignoring characters until '$' symbol
                file >> amount;
                totalIncome += amount;
            }
        }

        file.close();
        return totalIncome;
    }

private:
    string fileName;
    vector<unique_ptr<Transaction>> transactions;

    void loadExpensesFromFile()
    {
        ifstream file(fileName);
        if (!file.is_open())
        {
            cerr << "Warning: Unable to open the file for reading.\n";
            return;
        }

        string line;
        while (getline(file, line))
        {
            if (line == "[Expense]")
            {
                string date, description;
                double amount;
                getline(file, date);
                getline(file, description);
                file.ignore(1000, '$'); // Ignoring characters until '$' symbol
                file >> amount;

                unique_ptr<Transaction> transaction = make_unique<Expense>(date, description, -amount);
                transactions.push_back(move(transaction));
            }
            else if (line == "[Income]")
            {
                string date, description;
                double amount;
                getline(file, date);
                getline(file, description);
                file.ignore(1000, '$'); // Ignoring characters until '$' symbol
                file >> amount;

                unique_ptr<Transaction> transaction = make_unique<Income>(date, description, amount);
                transactions.push_back(move(transaction));
            }
        }

        file.close();
        cout << "Transactions loaded from " << fileName << "\n";
    }

    void saveExpensesToFile() const
    {
        ofstream file(fileName);

        if (!file.is_open())
        {
            cerr << "Error: Unable to open the file for writing.\n";
            return;
        }

        for (const auto &transaction : transactions)
        {
            file << "[" << transaction->getType() << "]\n";
            file << "Date: " << transaction->getDate() << "\n";
            file << "Description: " << transaction->getDescription() << "\n";
            file << "Amount: $" << transaction->getAmount() << "\n";
        }

        file.close();
    }
};

int main()
{
    ExpenseTracker tracker("expenditure.txt");
    cout << endl
         << "--------********WELCOME TO FINANCE TRACKER SYSTEM********-------" << endl;
    while (true)
    {
        cout << endl
             << "Options:\n";
        cout << "1. Add Expense\n";
        cout << "2. Add Income\n";
        cout << "3. List Transactions\n";
        cout << "4. Calculate Total Expenses\n";
        cout << "5. Calculate Total Income\n";
        cout << "6. Calculate Total Earnings\n";
        cout << "7. Exit\n";
        cout << "Enter your choice: ";

        int choice;
        cin >> choice;

        switch (choice)
        {
        case 1:
        case 2:
        {
            string date, description;
            double amount;

            cout << "Enter date (YYYY-MM-DD): ";
            cin >> date;
            cout << "Enter description: ";
            cin.ignore(); // Consume newline
            getline(cin, description);
            cout << "Enter amount: $";
            cin >> amount;

            unique_ptr<Transaction> transaction;
            if (choice == 1)
            {
                transaction = make_unique<Expense>(date, description, amount);
            }
            else
            {
                transaction = make_unique<Income>(date, description, amount);
            }
            tracker.addTransaction(move(transaction));
            break;
        }
        case 3:
            tracker.listTransactions();
            break;
        case 4:
            cout << "Total Expenses: $" << tracker.calculateTotalExpenses() << "\n";
            break;
        case 5:
            cout << "Total Income: $" << tracker.calculateTotalIncome() << endl;
            break;

        case 6:
            cout << "Total Earnings: $" << tracker.calculateTotalIncome() - (tracker.calculateTotalExpenses()) << endl;
            break;
        case 7:
            cout << "Successfully exited\n"
                 << endl;
            return 0;
        default:
            cout << "Invalid choice. Try again.\n";
        }
    }

    return 0;
}
