#ifndef AXIS_SOURCE_HPP
#define AXIS_SOURCE_HPP

// Output an AXI Stream from a vector of vectors
// N.B. Currently this does not support any kind of reset

#include "ClockGen.hpp"
#include <vector>

template <class dataT> class AXISSource
{
public:
	AXISSource(ClockGen &clkIn,
		const vluint8_t &readyIn, vluint8_t & validIn, vluint8_t &lastIn,
		dataT &dataIn, std::vector<std::vector<dataT>> vecIn)
		:clk(clkIn), ready(readyIn), valid(validIn), last(lastIn),
		 data(dataIn), vec(vecIn)
	{
		//Initiailise outputs
		valid = 1;
		assert(vec[0].size() > 0);
		data = vec[0][0];
		last = (vec[0].size() == 1);
	};
	// Returns true if we are done
	bool done(void) const {return (vec.size() == 0);};

	void eval(void)
	{
		if(clk.getEvent() == ClockGen::Event::RISING)
		{
			if(ready && valid)
			{
				last = 0; // Reset last flag
				assert(vec[0].size() != 0);
				vec[0].erase(vec[0].begin()); //Get rid of the word we output

				// Get next word onto front
				if(vec[0].size() == 0)
				{
					// If that was the end of a packet, pop it off
					vec.erase(vec.begin());
					if(vec.size() == 0)
					{
						// That was the last packet. We are done
						valid = 0;
						return;
					} else {
					// It is illegal for the newly popped packet to be empty
					assert(vec[0].size() != 0);
					}
				}

				//Setup outputs
				data = vec[0][0];
				last = (vec[0].size() == 1);
			}
		}
	}

private:
	ClockGen &clk;
	const vluint8_t &ready;
	vluint8_t &valid;
	vluint8_t &last;
	dataT &data;

	std::vector<std::vector<dataT>> vec;
};

#endif
