#include <iostream>
#include <future>
#include <thread>
#include <string>
#include <syncstream>

using namespace std;
using namespace std::chrono;

void slow(const string& name) {
    this_thread::sleep_for(seconds(7));
    osyncstream(cout) << name << " done" << endl;
}

void quick(const string& name) {
    this_thread::sleep_for(seconds(1));
    osyncstream(cout) << name << " done" << endl;
}

void work() {
    auto start = high_resolution_clock::now();

    // 1. А2, А1, B2 одночасно
    future<void> a2 = async(launch::async, slow, "A2");
    future<void> a1 = async(launch::async, quick, "A1");
    future<void> b2 = async(launch::async, slow, "B2");

    // Чекаємо завершення цих обчислень
    a1.get();
    a2.get();
    b2.get();

    // 2. B1, A3 одночасно
    future<void> b1 = async(launch::async, quick, "B1");
    future<void> a3 = async(launch::async, quick, "A3");

    // Чекаємо завершення цих обчислень
    b1.get();
    a3.get();

    // 3. A4, C одночасно
    future<void> a4 = async(launch::async, quick, "A4");
    future<void> c = async(launch::async, quick, "C");

    a4.get();
    c.get();

    auto end = high_resolution_clock::now();
    duration<double> elapsed = end - start;

    osyncstream(cout) << "Time spent on realisation: " << elapsed.count() << " seconds" << endl;
    osyncstream(cout) << "Work is done!" << endl;
}

int main() {
    work();
    return 0;
}
