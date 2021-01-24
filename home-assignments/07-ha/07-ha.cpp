/*
* Tatu Alatalo / N4927
* Home-assignment 7:
* 1. Create a new console app. 
* 2. Declare a class Sensor.
*   - Sensor uses a thread internally to read sensor values.
*   - Simulate sensor values by generating random numbers in between 0-4095
* 3. Declare a class SensorReader.
*   - SensorReader allocates 4 sensors and starts a thread to read sensor values.
*   - SensorReader thread can be stopped by a function call to SensorReader::stop.
* 4. Assume situation that reader is only interested of sensor values larger than 4000.
*   - Implement future-promise; reader will wait for the result of sensor value over 4000.
*   - When SensorReader gets a sensor value over 4000, print it to the console.
* 5. On your main function, allocate a SensorReader and let it run until user presses a key in console.
*/

#include <iostream>
#include <thread>
#include <random>          
#include <future>           // promise/future
#include <conio.h>          // _getch()
#define RESET   "\033[0m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"

using namespace std;

class Sensor {
public:
    void simulate(promise<int>& v) {
        // set the seed and random
        random_device rnd;
        mt19937 gen(rnd());
        uniform_int_distribution<> distribution(0, 4095);
        // get random value from between 0-4095
        int value = distribution(gen);
        // set the value to the promise
        v.set_value(value);
    }
};

class SensorReader {
public:
    SensorReader() : running(false), readingCount(0) {
    }
   
    void start() {
        running = true;
        
        while (running) {
            // present promise integers
            promise<int>    p1;
            promise<int>    p2;
            promise<int>    p3;
            promise<int>    p4;

            // sensors use thread to simulate and read the number
            thread t1(&Sensor::simulate, &s1, ref(p1));
            thread t2(&Sensor::simulate, &s2, ref(p2));
            thread t3(&Sensor::simulate, &s3, ref(p3));
            thread t4(&Sensor::simulate, &s4, ref(p4));

            // readings are saved to int variables
            int reading1 = p1.get_future().get();
            int reading2 = p2.get_future().get();
            int reading3 = p3.get_future().get();
            int reading4 = p4.get_future().get();

            // increase readingCount
            readingCount++;
            // readed values are then compared if they're over 4000
            if (reading1 > 4000) {
                cout << "Reading #" << readingCount << ": Sensor1 value is over 4000! (" << YELLOW << reading1 << RESET << ")" << endl;
            }

            readingCount++;
            if (reading2 > 4000) {
                cout << "Reading #" << readingCount << ": Sensor2 value is over 4000! (" << YELLOW << reading2 << RESET << ")" << endl;
            }

            readingCount++;
            if (reading3 > 4000) {
                cout << "Reading #" << readingCount << ": Sensor3 value is over 4000! (" << YELLOW << reading3 << RESET << ")" << endl;
            }

            readingCount++;
            if (reading4 > 4000) {
                cout << "Reading #" << readingCount << ": Sensor4 value is over 4000! (" << YELLOW << reading4 << RESET << ")" << endl;
            }

            // finally join threads
            t1.join();
            t2.join();
            t3.join();
            t4.join();
        }
    }

    void startReading(thread& t) {
        t = thread(&SensorReader::start, this);
    }

    void stop() {
        running = false;
    }

private:
    Sensor          s1;
    Sensor          s2;
    Sensor          s3;
    Sensor          s4;
    bool            running;
    int             readingCount;
    
};

int main()
{
    SensorReader s;
    thread t;

    // SensorReader starts reading sensor-values with one thread
    s.startReading(ref(t));
    // press any key to stop the SensorReader
    if (_getch() != 0) {
        s.stop();
    }
    // on exit join thread
    t.join();
    return 0;
}