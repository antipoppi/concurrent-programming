/* 
* Tatu Alatalo / N4927
* Home-assignment 3:
* 1. Create a new console app.
* 2. Declare a std::vector as an dynamic array of numbers
* 3. Create two threads, another will add numbers to the vector and another will remove them
* 4. Synchronize the threads with a Mutex.
* 5. Copy your source code to the Moodle return box.
*/

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

using namespace std;

// make mutex globally available for entire app
mutex mutex1;
vector<int> v;

// adds number to the vector
void addNumber(vector<int>& v) {
	if (mutex1.try_lock()) {
		v.push_back(rand());
		// cout << "added" << endl;
		mutex1.unlock();
	}
}

// removes number from the start of the vector
void delNumber(vector<int>& v) {
	if (!v.empty()) {
		if (mutex1.try_lock()) {
			v.erase(v.begin());
			// cout << "erased" << endl;
			mutex1.unlock();
		}
	}
}

void t1Func(vector<int>& v) {
	// worker function processes data indefinitely
	while (1) {
		addNumber(v);
	}
}

void t2Func(vector<int>& v) {
	// worker function processes data indefinitely
	while (1) {
		delNumber(v);
	}
}

int main() {
	// start the threads to work on the data
	thread t1(t1Func, ref(v));
	thread t2(t2Func, ref(v));

	// main loop keeps printing the size of vector
	// there's a chance the size increases because processor could give more turns to the adding thread than removing
	// if removing comments from "print text -command" in the methods this can be verified
	while (1) {
		mutex1.lock();
		cout << v.size() << endl;
		mutex1.unlock();
	}
	return 0;
}