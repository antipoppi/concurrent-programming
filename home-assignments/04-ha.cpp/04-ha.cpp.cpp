/*
* Tatu Alatalo / N4927
* Home-assignment 4:
* 1. Create a class that simulates a bank account operations and holds the current balance of the account.
* 2. Add member functions for deposit, withdraw and balance.
* 3. Declare two bank account objects of aforementioned class.
* 4. Start 4 threads that will make random deposits and withdraws between these two accounts.
* 5. Protect the bank account data with mutexes and avoid deadlocks.
*/

#include <iostream>
#include <thread>
#include <mutex>
#include <random>
#include <chrono>
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"

using namespace std;

class BankAccount {
public:
    // Bank account has default balance of 1000 euros
    BankAccount() : m_accountBalance(1000) {
    }

    mutex       m_mutex;

    void deposit(int deposit) {
        m_accountBalance += deposit;
    }

    void withdraw(int withdraw) {
        m_accountBalance -= withdraw;
    }

    int balance() {
        return m_accountBalance;
    }

private:
    int         m_accountBalance;
};

void helpFunc(BankAccount& bAcc1, BankAccount& bAcc2, int increment) {
    // set the random seed with the thread increment
    srand((unsigned int)time(0) + increment);
    bool outOfMoney = false;

    while (!outOfMoney) {
        
        int rnd = rand() % 2 + 1;
        int amount = rand() % 1000 + 1;

        // lock bank accounts for possible transaction
        bAcc1.m_mutex.lock();
        bAcc2.m_mutex.lock();

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
                // else make a transaction and print info
                cout << GREEN << "Thread ID:" << this_thread::get_id() << RESET << " Withdraw " << amount << " euros of "<< bAcc1.balance() << " from bank account #1" << endl;
                bAcc1.withdraw(amount);
                cout << GREEN << "Thread ID:" << this_thread::get_id() << RESET << " Deposit " << amount << " euros into bank account #2" << endl;
                bAcc2.deposit(amount);
                cout << "Bank account #1 balance is now " << bAcc1.balance() << " euros" << endl;
                cout << "Bank account #2 balance is now " << bAcc2.balance() << " euros" << endl;
                this_thread::sleep_for(chrono::milliseconds(200));
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
                // else make a transaction and print info
                cout << GREEN << "Thread ID:" << this_thread::get_id() << RESET << " Withdraw " << amount << " euros of " << bAcc2.balance() << " euros from bank account #2" << endl;
                bAcc2.withdraw(amount);
                cout << GREEN << "Thread ID:" << this_thread::get_id() << RESET << " Deposit " << amount << " euros into bank account #1" << endl;
                bAcc1.deposit(amount);
                cout << "Bank account #1 balance is now " << bAcc1.balance() << " euros" << endl;
                cout << "Bank account #2 balance is now " << bAcc2.balance() << " euros" << endl;
                this_thread::sleep_for(chrono::milliseconds(200));
            }
        }
        // unlock bank accounts now that the possible transaction is complete
        bAcc1.m_mutex.unlock();
        bAcc2.m_mutex.unlock();
    }
}

int main()
{
    int transactionCount = 0;
    BankAccount bAcc1, bAcc2;

    thread t1(helpFunc, ref(bAcc1), ref(bAcc2), 1);
    thread t2(helpFunc, ref(bAcc1), ref(bAcc2), 2);
    thread t3(helpFunc, ref(bAcc1), ref(bAcc2), 3);
    thread t4(helpFunc, ref(bAcc1), ref(bAcc2), 4);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    return 0;   
}