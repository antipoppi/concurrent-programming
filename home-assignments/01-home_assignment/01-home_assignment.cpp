// 01-home_assignment.cpp : This file contains the 'main' function. Program execution begins and ends there.
// 1. Make a new console app.
// 2. Detect amount of logical processors at app runtime.
// 3. Create as many threads as your computer has logical processors.
// 4. Each thread runs simultaneously a for loop for 1000 iterations and print the current loop value while running.
// 5. In main, wait for all threads to finish.
// 6. Copy the source code to Moodle return box.

#include <iostream>
#include <thread>
#include <vector>

using namespace std;

void runningNumber()
{
    for (int i = 0; i < 1000; i++)
    {
        cout << "Thread ID: " << this_thread::get_id() << ". Current iteration is " << i << endl;
    }
}
        

int main()
{
    // first get the amount of how many logical processors you have
    int lpCount = thread::hardware_concurrency();

    // create a vector to hold the threads
    vector<thread> threadVector;

    // create threads for amount of how many logical processors there are and put them to count into 1000 simultaneously
    for (int i = 0; i < lpCount; i++)
    {
        threadVector.push_back(thread(runningNumber));
    }
        
    // wait for all of them to be done and finish
    for (thread& th : threadVector)
    {
        if (th.joinable())
        {
            th.join();
        }
    }
        
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
