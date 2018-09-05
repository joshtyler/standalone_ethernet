#include <iostream>
#include <iomanip> //setw
#include "Veth_test.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

#include "ClockGen.hpp"
#include "DummyRMII.hpp"
#include "VerilatedModel.hpp"

int main(int argc, char** argv)
{
	const bool recordVcd = true;

	VerilatedModel<Veth_test> uut(argc,argv,recordVcd);

	ClockGen clk(uut.getTime(), 1e-9, 50e6);
	DummyRMII rmii(clk, uut.uut->txd, uut.uut->tx_en,
		uut.uut->rxd, uut.uut->crs_dv, uut.uut->rx_er);


	uut.addPeripheral(&rmii);
	ClockBind clkDriver(clk,uut.uut->clk);
	uut.addClock(&clkDriver);

	while(true)
	{
		if(uut.eval() == false)
		{
			break;
		}

		// Break on timeout
		if(uut.getTime() == 100000)
		{
			std::cout << "Timed out" << std::endl;
			break;
		}
	}
	// Print first packet
	std::vector<std::vector<uint8_t>> data = rmii.getData();
	if(data.size() > 0)
	{
		std::cout << "First packet:" << std::hex;
		for(auto const & itm : data[0])
		{
			std::cout << std::setfill('0') << std::setw(2) << (int)itm << " ";
		}
		std::cout << std::dec << std::endl;
	}
}
