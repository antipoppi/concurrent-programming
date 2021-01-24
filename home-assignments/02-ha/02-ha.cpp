// 02-ha.cpp : This file contains the 'main' function. Program execution begins and ends there.
// 1. Create new console app.
// 2. Ask user of how many numbers to allocate.
// 3. Allocate an array of numbers and pass it to thread that sets array elements to random numbers.
// 4. In main, wait for the thread to finish and print the contents of the array.
// 5. Copy your source code to Moodle return box.

#include <iostream>
#include <thread>
#include <random>

using namespace std;

void fillArray(int n[], int v)
{
    // fill array with random number between 1-1000 and print the array
    for (int i = 0; i < v; i++)
    {
        n[i] = (rand() % 1000) + 1;
        // or if you want a number between 0 and max_integer just use
        // n[i] = rand();
    }
    cout << "Contents of the array: " << endl;
    for (int i = 0; i < v; i++)
    {
        cout << "Array[" << i << "]: " << n[i] << endl;
    }
}

int main()
{
    // ask the user how many numbers to allocate, save it to the variable and create an allocated array
    int value;
    cout << "Enter how many numbers to allocate: " << endl;
    cin >> value;
    int* numbers = new int[value];
    // pass fillArray function to the thread with array and value 
    thread t1(fillArray, numbers, value);
    if (t1.joinable())
    {
        t1.join();
    }
    // finally when done, delete the array
    delete[] numbers;
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
