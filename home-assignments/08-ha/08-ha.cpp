/*
* Tatu Alatalo / N4927
* Home-assignment 8:
* 1. Create a new console app.
* 2. Declare a class 'RandomNumberGenerator'. In class, specify enum of task type as LIGHT or HEAVY.
*   - If RandomNumberGenerator is LIGHT, it will generate 100 random numbers.
*   - If RandomNumberGenerator is HEAVY, it will generate 10 000 000 random numbers.
* 3. Allocate 100 RandomNumberGenerators and set the task type to LIGHT or HEAVY randomly.
* 4. Run the allocated RandomNumberGenerators as async tasks.
*   - If generator is LIGHT, use deferred launching.
*   - If generator is HEAVY, use async launching.
* 5. Measure how long it takes to complete all generators.
* 6. Modify your code to run all tasks with async launching.
* 7. Measure how long it takes to complete all generators.
* 8. Add comments into the source code of your findings. Which way is faster? Why?
*/

#include <iostream>
#include <thread>
#include <random>
#include <vector>
#include <chrono>
#include <future>


using namespace std;
using namespace std::chrono;

class RandomNumberGenerator {
public:
    enum TaskType { LIGHT, HEAVY };

    void Generate(TaskType type) {
        if (type == LIGHT) {
            auto f = async(launch::deferred, [&] { LightGenerate(); } );
            f.get(); 
        }
        if (type == HEAVY) {
            auto f = async(launch::async, [&] { HeavyGenerate(); } );
            f.get();
        }
    }

    void LightGenerate() {
        for (int i = 0; i < 100; i++)
        {
            int num = rand() % INT_MAX;
        }
    }
    
    void HeavyGenerate() {
        for (int i = 0; i < 10000000; i++)
        {
            int num = rand() % INT_MAX;
        }
    }
};

int main()
{
    // I did not use a seed to random for compare-reasons.

    vector<RandomNumberGenerator> generators;
    int count = 0;

    // allocate 100 RandomNumberGenerators in to a vector and set their random task
    for (int i = 0; i < 100; i++)
    {
        RandomNumberGenerator r;
        generators.push_back(r);
    }

    // loop 5 times
    while (count < 5) {
        count++;
        // get start time
        system_clock::time_point start = system_clock::now();

        // loop through vector and start generating
        for (RandomNumberGenerator r : generators) {
            r.Generate(RandomNumberGenerator::TaskType(rand() % 2));
        }

        // get end time and print result
        system_clock::time_point end = system_clock::now();
        cout << "Total time = " << duration_cast<seconds>(end - start).count() << " seconds" << endl;
    }
    
    // I looped main for 5 times and got the next results:
    // deferred/async   35s, 39s, 39s, 37s, 34s 
    // async/async      33s, 35s, 34s, 35s, 33s

    // Result: Async seems to be faster. 
    // That seems to be because launch::deferred is executed after future::get is called.
    // So it starts later in the function compared to launch::async what invokes a new thread right away.
    
    return 0;
}
