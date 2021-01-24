/*
* Tatu Alatalo / N4927
* Home-assignment 6:
* 1. Take the code from home assignment 4 to modify it for this assignment.
* 2. Add function addInterest to bank account class.
* 3. After 100 deposit or withdraw operations, an addInterest function is called:
*       - addInterest adds 0.05 per cent interest to account balance (based on current balance).
* 4. Modify the app to operate lock-free using atomics.
* 
* Tips:
* 1. Its not as simple as changing balance to std::atomic as you will need computations in addInterest function
*    that are not supported by the atomic type (multipication, division of floating point values).
* 2. Your options are:
*       - use new atomic variable as a barrier or as a light weight DIY mutex.
*       - perform atomic operations to account balance using Compare and Swap technique with compare_exchange.. functions.
* 3. Avoid busy loops if using atomic variable for synchronizing.
*/

#include <iostream>
#include <thread>
#include <random>
#include <chrono>
#include <iomanip>
#include <atomic>
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"

using namespace std;

class BankAccount {
public:
    // Bank account has default balance of 1000 euros
    BankAccount() : m_accountBalance(1000) {

    }

    void deposit(int deposit) {
        // makes a CAS (compare-and-swap) technique for the deposit
        double holder = m_accountBalance;
        while (!m_accountBalance.compare_exchange_strong(holder, holder + deposit)){}
    }

    void withdraw(int withdraw) {
        // makes a CAS (compare-and-swap) technique for the withdraw
        double holder = m_accountBalance;
        while (!m_accountBalance.compare_exchange_strong(holder, holder - withdraw)) {}
    }

    double balance() {
        return m_accountBalance;
    }

    void addInterest() {
        // makes a CAS (compare-and-swap) technique for the addInterest
        double holder = m_accountBalance;
        while (!m_accountBalance.compare_exchange_strong(holder, holder * 1.0005)) {}
    }

private:
    atomic<double>          m_accountBalance;
};

void helpFunc(BankAccount& bAcc1, BankAccount& bAcc2, atomic<int>& transactionCount, int increment) {
    // set the random seed with the thread increment
    srand((unsigned int)time(0) + increment);
    bool outOfMoney = false;

    while (!outOfMoney) {

        int rnd = rand() % 2 + 1;
        // amount is a random number between 1 and 500€ (in home-assignment 4 it was between 1-1000)
        int amount = rand() % 500 + 1;

        // this makes the current thread to wait a little before it starts working
        // so there's some variance between them (workaround to avoid busy loops)
        // later threads are put to sleep so they can cleanly print to the console
        this_thread::sleep_for(chrono::milliseconds(amount * increment));

        // if rnd is 1 there will be a transaction from bAcc1 to bAcc2 and vice versa
        if (rnd == 1) {
            if (bAcc1.balance() < amount) {
                // if bank account #1 balance is smaller than the amount of the transaction, thread quits
                cout << GREEN << "Thread ID:" << this_thread::get_id() << RESET << " Bank account #1 cannot withdraw " << amount << " euros because it only has " << bAcc1.balance() << " euros" << endl;
                cout << "Bank account #1 would then be out of money" << endl;
                cout << RED << "Thread " << this_thread::get_id() << " will now exit..." << RESET << endl;
                this_thread::sleep_for(chrono::milliseconds(200));
                outOfMoney = true;
            }
            else {
                // else make a transaction, increase transaction count and print info
                cout << GREEN << "Thread ID:" << this_thread::get_id() << RESET << " Withdraw " << amount << " euros of " << bAcc1.balance() << " from bank account #1" << endl;
                bAcc1.withdraw(amount);
                this_thread::sleep_for(chrono::milliseconds(200));
                cout << GREEN << "Thread ID:" << this_thread::get_id() << RESET << " Deposit " << amount << " euros into bank account #2" << endl;
                bAcc2.deposit(amount);
                transactionCount++;
                cout << YELLOW << "Transaction #" << transactionCount << " done (" << this_thread::get_id() << ")" << RESET << endl;
                // if transaction count is 100, call addInterest()-memberfunction
                if (transactionCount == 100) {
                    bAcc1.addInterest();
                    bAcc2.addInterest();
                    cout << GREEN << "Interest of 0.05% added to the accounts!" << RESET << endl;
                    cout << "Bank account #1 balance is now " << setprecision(2) << fixed << bAcc1.balance() << " euros" << endl;
                    cout << "Bank account #2 balance is now " << setprecision(2) << fixed << bAcc2.balance() << " euros" << endl;
                    this_thread::sleep_for(chrono::milliseconds(200));
                }
                else {
                    cout << "Bank account #1 balance is now " << bAcc1.balance() << " euros" << endl;
                    cout << "Bank account #2 balance is now " << bAcc2.balance() << " euros" << endl;
                    this_thread::sleep_for(chrono::milliseconds(amount));
                }
            }
        }
        else {
            if (bAcc2.balance() < amount) {
                // if bank account #2 balance is smaller than the amount of the transaction, thread quits
                cout << GREEN << "Thread ID:" << this_thread::get_id() << RESET << " Bank account #2 cannot withdraw " << amount << " euros because it only has " << bAcc2.balance() << " euros" << endl;
                cout << "Bank account #2 would then be out of money" << endl;
                cout << RED << "Thread " << this_thread::get_id() << " will now exit..." << RESET << endl;
                this_thread::sleep_for(chrono::milliseconds(200));
                outOfMoney = true;
            }
            else {
                // else make a transaction, increase transaction count and print info
                cout << GREEN << "Thread ID:" << this_thread::get_id() << RESET << " Withdraw " << amount << " euros of " << bAcc2.balance() << " euros from bank account #2" << endl;             
                bAcc2.withdraw(amount);
                this_thread::sleep_for(chrono::milliseconds(200));
                cout << GREEN << "Thread ID:" << this_thread::get_id() << RESET << " Deposit " << amount << " euros into bank account #1" << endl;
                bAcc1.deposit(amount);
                transactionCount++;
                cout << YELLOW << "Transaction #" << transactionCount << " done (" << this_thread::get_id() << ")" << RESET << endl;
                // if transaction count is 100, call addInterest()-memberfunction
                if (transactionCount == 100) {
                    bAcc1.addInterest();
                    bAcc2.addInterest();
                    cout << GREEN << "Interest of 0.05% added to the accounts!" << RESET << endl;
                    cout << "Bank account #1 balance is now " << setprecision(2) << fixed << bAcc1.balance() << " euros" << endl;
                    cout << "Bank account #2 balance is now " << setprecision(2) << fixed << bAcc2.balance() << " euros" << endl;
                    this_thread::sleep_for(chrono::milliseconds(200));
                }
                else {
                    cout << "Bank account #1 balance is now " << bAcc1.balance() << " euros" << endl;
                    cout << "Bank account #2 balance is now " << bAcc2.balance() << " euros" << endl;
                    this_thread::sleep_for(chrono::milliseconds(amount * increment));
                }
            }
        }
    }
}

int main()
{
    // NOTE: The transaction count is set to 95 so the addInterest-function can be called earlier
    atomic<int> transactionCount = 95;
    BankAccount bAcc1, bAcc2;

    thread t1(helpFunc, ref(bAcc1), ref(bAcc2), ref(transactionCount), 1);
    thread t2(helpFunc, ref(bAcc1), ref(bAcc2), ref(transactionCount), 2);
    thread t3(helpFunc, ref(bAcc1), ref(bAcc2), ref(transactionCount), 3);
    thread t4(helpFunc, ref(bAcc1), ref(bAcc2), ref(transactionCount), 4);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    return 0;
}