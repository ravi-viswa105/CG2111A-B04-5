#include <iostream>
#include <chrono>
using namespace std;
using namespace std::chrono;
extern high_resolution_clock::time_point start;

microseconds::rep current_time();
void set_current_time();
