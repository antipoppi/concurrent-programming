/*
* Tatu Alatalo / N4927
* Home-assignment 5:
* 1. Create a new console app.
* 2. Create a worker thread with a long running task.
* 3. Use 5 condition variables within the worker thread to notify the main thread of the progress.
*    - Each condition variable amounts to 20% of the total task done
* 4. In main thread, wait for the condition variables to complete and print the progress of the worker thread.
* 5. Copy your source code to the Moodle return box.
*/
#include <iostream>
#include <thread>
#include <vector>
#include<mutex>
#include<condition_variable>
#include<functional>
#define RESET   "\033[0m"
#define GREEN   "\033[32m"

using namespace std;

class Application
{
public:
    Application() : m_bDataWorked1(false),
                    m_bDataWorked2(false),
                    m_bDataWorked3(false),
                    m_bDataWorked4(false),
                    m_bDataWorked5(false)
    {
        // set the random seed
        srand((unsigned int)time(0));
    }

    mutex               m_mutex;
    condition_variable  m_condVar1;
    condition_variable  m_condVar2;
    condition_variable  m_condVar3;
    condition_variable  m_condVar4;
    condition_variable  m_condVar5;

    void printData() {
        for (size_t i = 0; i < m_data.size();i++)
        {
            cout << m_data.at(i);
            this_thread::sleep_for(chrono::milliseconds(1)); 
        }
    }

    void workerFunc()
    {
        
        // simulate work
        for (int i = 0; i <= 100; i += 10) {
            // artificially slow down working process before locking so the print-cmd has more time to print the progress into the console
            this_thread::sleep_for(chrono::milliseconds(100));

            // protect the data with mutex
            lock_guard<mutex> guard(m_mutex);

            for (int j = 0; j < 10; j++) {
                m_data.push_back((char)(rand() % 64 + 33));
            }
            
            if (i == 20) {
                // print current data and set m_bDataWorked1 to true 
                // to indicate that the data is 20% worked
                // finally notify condition variable1 in the main-thread and let it print the progress into the console
                printData();
                cout << endl;
                m_bDataWorked1 = true;
                m_condVar1.notify_one();
                
            }
            else if (i == 40) {
                printData();
                cout << endl;
                m_bDataWorked2 = true;
                m_condVar2.notify_one();
            }
            else if (i == 60) {
                printData();
                cout << endl;
                m_bDataWorked3 = true;
                m_condVar3.notify_one();
            }
            else if (i == 80) {
                printData();
                cout << endl;
                m_bDataWorked4 = true;
                m_condVar4.notify_one();
            }
            else if (i == 100) {
                printData();
                m_bDataWorked5 = true;
                m_condVar5.notify_one();
            }    
        }
    }

    bool isDataWorked1() const {
        return m_bDataWorked1;
    }
    bool isDataWorked2() const {
        return m_bDataWorked2;
    }
    bool isDataWorked3() const {
        return m_bDataWorked3;
    }
    bool isDataWorked4() const {
        return m_bDataWorked4;
    }
    bool isDataWorked5() const {
        return m_bDataWorked5;
    }
    
private:
    vector<char>        m_data;
    bool                m_bDataWorked1;
    bool                m_bDataWorked2;
    bool                m_bDataWorked3;
    bool                m_bDataWorked4;
    bool                m_bDataWorked5;
};

int main()
{
    Application app;

    thread thread1(&Application::workerFunc, &app);

    // start working
    cout << GREEN << "Worker has started working" << RESET << endl;

    // lock with unieque_lock and wait for the signal
    unique_lock<mutex> mutexLock1(app.m_mutex);
    app.m_condVar1.wait(mutexLock1, bind(&Application::isDataWorked1, &app));
    // signal received and print the progress to the console
    cout << GREEN << "20% done now" << RESET <<endl;
    // unlock the mutex
    mutexLock1.unlock();

    unique_lock<mutex> mutexLock2(app.m_mutex);
    app.m_condVar2.wait(mutexLock2, bind(&Application::isDataWorked2, &app));
    cout << GREEN << "40% done now" << RESET << endl;
    mutexLock2.unlock();

    unique_lock<mutex> mutexLock3(app.m_mutex);
    app.m_condVar3.wait(mutexLock3, bind(&Application::isDataWorked3, &app));
    cout << GREEN << "60% done now" << RESET << endl;
    mutexLock3.unlock();
    
    unique_lock<mutex> mutexLock4(app.m_mutex);
    app.m_condVar4.wait(mutexLock4, bind(&Application::isDataWorked4, &app));
    cout << GREEN << "80% done now" << RESET << endl;
    mutexLock4.unlock();
    
    unique_lock<mutex> mutexLock5(app.m_mutex);
    app.m_condVar5.wait(mutexLock5, bind(&Application::isDataWorked5, &app));
    cout << endl;
    cout << GREEN << "100% done now" << RESET << endl;
    mutexLock5.unlock();
    
    thread1.join();

    return 0;
}