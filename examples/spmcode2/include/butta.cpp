#include "hpc_helpers.hpp"

#include <unistd.h>

int main() {

	TIMERSTART(primo);
	usleep(1000);
	TIMERSTOP(primo);


	TIMERSTART(secondo);
	usleep(10000);
	TIMERSTOP(secondo);


	TIMERSUM(primo,secondo);

}
	
