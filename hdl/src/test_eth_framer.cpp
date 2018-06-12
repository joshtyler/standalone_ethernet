#include <iostream>
#include <iomanip> //setw
#include "Veth_framer.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

#include "ClockGen.hpp"
#include "AXISSink.hpp"
#include "AXISSource.hpp"


int main(int argc, char** argv)
{
	bool recordVcdTrace = true;
	VerilatedVcdC* tfp = NULL;

	Verilated::commandArgs(argc, argv);

	Veth_framer* top = new Veth_framer;
	vluint64_t time = 0;

	if (recordVcdTrace)
    {
        Verilated::traceEverOn(true);

        tfp = new VerilatedVcdC;
        top->trace(tfp, 99);

        std::string vcdname = argv[0];
        vcdname += ".vcd";
        std::cout << vcdname << std::endl;
        tfp->open(vcdname.c_str());
    }

	ClockGen clk(time, 1e-9, 100e6);
	AXISSink<vluint8_t> outAxis(clk, top->out_axis_tready,
		top->out_axis_tvalid, top->out_axis_tlast, top->out_axis_tdata);

	std::vector<std::vector<vluint8_t>> payload = {{0x0,0x1,0x2,0x3}};
	AXISSource<vluint8_t> payloadAxis(clk, top->payload_axis_tready,
		top->payload_axis_tvalid, top->payload_axis_tlast, top->payload_axis_tdata,
		payload);

    top->ethertype = 0x0800; // IP
    top->src_mac = 0x010203040506;
    top->dst_mac = 0xf0e0d0c0b0a0;
    top->payload_axis_tvalid = 1;

    top->out_axis_tready = 1;

	while(!Verilated::gotFinish())
	{
		//std::cout << "Time: " << time << std::endl;
		//std::cout << "Clock: " << clk.getVal() <<", " << clk.eventToStr(clk.getEvent()) << std::endl << std::endl;

		//Toggle clock
		top-> clk = clk.getVal();
		outAxis.eval();
		payloadAxis.eval();
		
		top->eval();

		//Set sresetn
		if(time > 0 && time < 10)
		{
			//std::cout << "Setting 0" << std::endl;
			top->sresetn = 0;
		} else {
			top->sresetn = 1;
		}

		//Add this to the dump
		if (tfp != NULL)
		{
		    tfp->dump(time);
		}
		//std::cout << "top->out_axis_tvalid: " << (int)top->out_axis_tvalid << std::endl;

		// Break if we have a whole packet
		// (or after a timeout)
		if(time == 10000)
		{
			std::cout << "Timed out" << std::endl;
			break;
		}
		if(outAxis.getTlastCount() == 1)
		{
			break;
		}

		time++;
	}
	// Print first packet
	std::vector<std::vector<vluint8_t>> data = outAxis.getData();
	std::cout << "First packet:" << std::hex ;
	for(auto const & itm : data[0])
	{
		std::cout << std::setfill('0') << std::setw(2) << (int)itm << " ";
	}
	std::cout << std::dec << std::endl;

    if (tfp != NULL)
    {
        tfp->close();
        delete tfp;
    }

}
