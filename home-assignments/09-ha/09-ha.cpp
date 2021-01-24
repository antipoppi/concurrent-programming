/*
* Tatu Alatalo / N4927
* Home-assignment 9:
* Airport
* 4 people are at the airport. To prepare for departure,
* each of them has to first scan their boarding pass (which takes 1 min),
* and then to do the security check (which takes 10 minutes).
*
* 1. Assume that there is only one machine for scanning the boarding pass and only one security line.
*    Explain why this pipeline is unbalanced. Compute its throughput.
* 2. Now assume that there are 2 security lines. Which is the new throughput?
* 3. If there were 4 security lines opened, would the pipeline be balanced?
*
* Implement a test program to verify your findings.
* Tip: You can simulate the throughput faster than 1/10 minutes.
* But after simulation, show the results based on original 1/10 minutes.
*/

#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#define RESET   "\033[0m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define PURPLE  "\033[35m"
#define CYAN    "\033[36m"

using namespace std;
using namespace std::chrono;

class Person {
public:
    string  name;
    system_clock::time_point start;
    system_clock::time_point end;

};

class BoardingPassScanner {
public:
    mutex   bps_mutex;

    void ScanBoardingPass(Person& p) {
        // BoardingPass  scanner is locked with the mutex while in use so other people can't enter
        lock_guard<mutex> guard(bps_mutex);
        cout << p.name << " is scanning the boarding pass" << endl;
        // I'm using 1 second here for sleeping but refer it as a 1 minute later on
        this_thread::sleep_for(chrono::seconds(1));
        cout << p.name << " finished scanning the boarding pass" << endl;
    }
};

class SecurityCheck {
public:
    mutex   sl_mutex;
    int     id;

    void DoSecurityCheck(Person& p) {
        // SecurityCheck is locked with the mutex while in use so other people can't enter
        lock_guard<mutex> guard(sl_mutex);
        cout << PURPLE << p.name << " entered to the security check #" << id << RESET << endl;
        // I'm using 10 second here for sleeping but refer it as a 10 minute later on
        this_thread::sleep_for(chrono::seconds(10));
        p.end = system_clock::now();
        cout << GREEN << p.name << " is free to go" << RESET << endl;
        cout << YELLOW << p.name << " total waiting time = " << duration_cast<seconds>(p.end - p.start).count() << " minutes" << RESET << endl;
    }
};

int main()
{
    // set objects and member values
    BoardingPassScanner bps;
    SecurityCheck ss1;
    SecurityCheck ss2;
    SecurityCheck ss3;
    SecurityCheck ss4;
    Person p1, p2, p3, p4;
    p1.name = "Person 1";
    p2.name = "Person 2";
    p3.name = "Person 3";
    p4.name = "Person 4";
    ss1.id = 1;
    ss2.id = 2;
    ss3.id = 3;
    ss4.id = 4;
   
    // CASE 1
    
    cout << CYAN << "CASE #1" << RESET << endl;
    cout << CYAN << "- Only one security line" << RESET << endl;

    // set persons starting time
    p1.start = system_clock::now();
    p2.start = system_clock::now();
    p3.start = system_clock::now();
    p4.start = system_clock::now();

    // person 1 goes to the BoardingPass scanner
    thread case1_t1(&BoardingPassScanner::ScanBoardingPass, &bps, ref(p1));
    // this_thread::sleep_for() is just to make sure that thread starts at the correct order
    this_thread::sleep_for(chrono::milliseconds(10));
    // person 2, 3 and 4 waits for the scanner
    thread case1_t2(&BoardingPassScanner::ScanBoardingPass, &bps, ref(p2));
    this_thread::sleep_for(chrono::milliseconds(10));
    thread case1_t3(&BoardingPassScanner::ScanBoardingPass, &bps, ref(p3));
    this_thread::sleep_for(chrono::milliseconds(10));
    thread case1_t4(&BoardingPassScanner::ScanBoardingPass, &bps, ref(p4));

    // when person 1 has scanned the boarding pass it goes to the SecurityCheck #1
    case1_t1.join();
    thread case1_t5(&SecurityCheck::DoSecurityCheck, &ss1, ref(p1));
    // when person 2 is done it waits for the SecurityCheck #1 to be free
    case1_t2.join();
    thread case1_t6(&SecurityCheck::DoSecurityCheck, &ss1, ref(p2));
    // same with person 3 and 4
    case1_t3.join();
    thread case1_t7(&SecurityCheck::DoSecurityCheck, &ss1, ref(p3));
    case1_t4.join();
    thread case1_t8(&SecurityCheck::DoSecurityCheck, &ss1, ref(p4));
    // finally join remaining threads
    case1_t5.join();
    case1_t6.join();
    case1_t7.join();
    case1_t8.join();

    /* Result:  Person 1 total waiting time 11 minutes
    *           Person 2 total waiting time 21 minutes
    *           Person 3 total waiting time 31 minutes
    *           Person 4 total waiting time 41 minutes
    * 
    * Comment:  People are mostly lining up at the security line because that's the bottle neck
    */

    // CASE 2
    
    cout << CYAN << "\nCASE #2" << RESET << endl;
    cout << CYAN << "- Two security lines" << RESET << endl;

    // Set persons starting time
    p1.start = system_clock::now();
    p2.start = system_clock::now();
    p3.start = system_clock::now();
    p4.start = system_clock::now();

    // Person 1 goes to the BoardingPass scanner
    thread case2_t1(&BoardingPassScanner::ScanBoardingPass, &bps, ref(p1));
    // this_thread::sleep_for() is just to make sure that thread starts at the correct order
    this_thread::sleep_for(chrono::milliseconds(10));
    // Person 2, 3 and 4 waits for the scanner to be free
    thread case2_t2(&BoardingPassScanner::ScanBoardingPass, &bps, ref(p2));
    this_thread::sleep_for(chrono::milliseconds(10));
    thread case2_t3(&BoardingPassScanner::ScanBoardingPass, &bps, ref(p3));
    this_thread::sleep_for(chrono::milliseconds(10));
    thread case2_t4(&BoardingPassScanner::ScanBoardingPass, &bps, ref(p4));

    // When person 1 has scanned the boarding pass it goes to the SecurityCheck #1
    case2_t1.join();
    thread case2_t5(&SecurityCheck::DoSecurityCheck, &ss1, ref(p1));
    // Because the assignment didn't say how the person chooses the line, I chose the line myself. Lines will fill up evenly.
    // When person 2 has scanned the boarding pass it goes to the SecurityCheck #2 (which is currently empty)
    case2_t2.join();
    thread case2_t6(&SecurityCheck::DoSecurityCheck, &ss2, ref(p2));
    // When person 3 has scanned the boarding pass it goes waiting to the SecurityCheck #1
    case2_t3.join();
    thread case2_t7(&SecurityCheck::DoSecurityCheck, &ss1, ref(p3));
    // When person 4 has scanned the boarding pass it goes waiting to the SecurityCheck #2
    case2_t4.join();
    thread case2_t8(&SecurityCheck::DoSecurityCheck, &ss2, ref(p4));
    // Finally join remaining threads
    case2_t5.join();
    case2_t6.join();
    case2_t7.join();
    case2_t8.join();

    /* Result:  Person 1 total waiting time 11 minutes
    *           Person 2 total waiting time 12 minutes
    *           Person 3 total waiting time 21 minutes
    *           Person 4 total waiting time 22 minutes
    *
    * Comment:  Opening other line helped a little. People are still mostly lining up at the security line because that's the bottle neck.
    */
    
    // CASE 3

    cout << CYAN << "\nCASE #3" << RESET << endl;
    cout << CYAN << "- Four security lines" << RESET << endl;

    // Set persons starting time
    p1.start = system_clock::now();
    p2.start = system_clock::now();
    p3.start = system_clock::now();
    p4.start = system_clock::now();

    // Person 1 goes to the BoardingPass scanner
    thread case3_t1(&BoardingPassScanner::ScanBoardingPass, &bps, ref(p1));
    this_thread::sleep_for(chrono::milliseconds(10));
    // Person 2, 3 and 4 waits for the scanner to be free
    thread case3_t2(&BoardingPassScanner::ScanBoardingPass, &bps, ref(p2));
    this_thread::sleep_for(chrono::milliseconds(10));
    thread case3_t3(&BoardingPassScanner::ScanBoardingPass, &bps, ref(p3));
    this_thread::sleep_for(chrono::milliseconds(10));
    thread case3_t4(&BoardingPassScanner::ScanBoardingPass, &bps, ref(p4));

    // When person 1 has scanned the boarding pass it goes to the SecurityCheck #1
    case3_t1.join();
    thread case3_t5(&SecurityCheck::DoSecurityCheck, &ss1, ref(p1));
    // When person 2 has scanned the boarding pass it goes to the SecurityCheck #2 (which is empty)
    case3_t2.join();
    thread case3_t6(&SecurityCheck::DoSecurityCheck, &ss2, ref(p2));
    // When person 3 has scanned the boarding pass it goes to the SecurityCheck #2 (which is empty)
    case3_t3.join();
    thread case3_t7(&SecurityCheck::DoSecurityCheck, &ss3, ref(p3));
    // When person 4 has scanned the boarding pass it goes to the SecurityCheck #2 (which is empty)
    case3_t4.join();
    thread case3_t8(&SecurityCheck::DoSecurityCheck, &ss4, ref(p4));
    // join remaining threads
    case3_t5.join();
    case3_t6.join();
    case3_t7.join();
    case3_t8.join();

    /* Result:  Person 1 total waiting time 11 minutes
    *           Person 2 total waiting time 12 minutes
    *           Person 3 total waiting time 13 minutes
    *           Person 4 total waiting time 14 minutes
    *
    * Comment:  Opening 4 lines helps a lot. As there are 4 lines people only had to wait at the Boarding Pass scanner.
    */

    return 0;
}
