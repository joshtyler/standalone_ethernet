#ifndef AXIS_SINK_HPP
#define AXIS_SINK_HPP

// Receive an AXIS stream and save it to a std::vector
// N.B. Currently this does not support any kind of reset

#include "ClockGen.hpp"
#include <vector>

template <class dataT> class AXISSink
{
public:
	AXISSink(ClockGen &clkIn,
		vluint8_t &readyIn, const vluint8_t & validIn, const vluint8_t &lastIn,
		const dataT &dataIn)
		:clk(clkIn), ready(readyIn), valid(validIn), last(lastIn),
		 data(dataIn)
	{
		//Push empty vector so that the first element has something to add to
		vec.push_back(std::vector<dataT>());

		//Always be ready
		ready = 1;
	};
	// Data is returned as a vector of vectors
	// Each element in the base vector is a packet
	// Each element in the subvector is a word
	std::vector<std::vector<dataT>> getData(void) {return vec;};

	//Return number of times tlast has been received
	unsigned int getTlastCount(void) const {return vec.size()-1;};

	void eval(void)
	{
		if(clk.getEvent() == ClockGen::Event::RISING)
		{
			//std::cout << "Got clk rising edge, ready:" << (int)ready << " valid:" << (int)valid << std::endl;
			if(ready && valid)
			{
				//std::cout << "Pushing" << std::endl;
				vec[vec.size()-1].push_back(data);
				if(last)
				{
					vec.push_back(std::vector<dataT>());
				}
			}
		}
	}

private:
	ClockGen &clk;
	vluint8_t &ready;
	const vluint8_t &valid;
	const vluint8_t &last;
	const dataT &data;

	std::vector<std::vector<dataT>> vec;
};

#endif
