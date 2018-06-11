// Generate a clock for verilator testbenches
// Just takes a reference to the current time
#include "verilated.h"

class ClockGen
{
public:
	enum class Event {NONE, RISING, FALLING};
	ClockGen(vluint64_t &count_in, double resolution, double freq)
		:count(count_in), ticksPerClock((1.0/freq)*(1.0/resolution)) {};
	bool getVal(void) {updateTime(); return val;};
	Event getEvent(void) {updateTime(); return event;}
private:
	vluint64_t &count;
	bool val;
	Event event;
	const unsigned int ticksPerClock;

 	void updateTime(void)
	{
		// Fall on remainder = 0
		// Rise on remainder >= ticksPerClock/2

		unsigned int remainder = count % ticksPerClock;
		event = Event::NONE;

		// Set val
		if(remainder < (ticksPerClock/2))
		{
			val = false;
			if(remainder = 0)
			{
				event = Event::FALLING;
			}
		} else {
			val = true;
			if (remainder == ticksPerClock/2)
			{
				event = Event::RISING;
			}
		}
	}
};
