#include "time.h"

microseconds::rep current_time(){
        auto duration = duration_cast<microseconds>(high_resolution_clock::now() - start);
        microseconds::rep amt = duration.count();
        return amt;
}

void set_current_time(){
	start = high_resolution_clock::now();
}
