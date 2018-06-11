#include <iostream>
#include "Veth_framer.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

#include "ClockGen.hpp"


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

    top->ethertype = 0x0800; // IP
    top->src_mac = 0x010203040506;
    top->dst_mac = 0xf0e0d0c0b0a0;
    top->payload_axis_tvalid = 1;

    top->out_axis_tready = 1;

	while(!Verilated::gotFinish())
	{
		//Toggle clock
		top-> clk = clk.getVal();
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
		time++;
		if(time == 1000)
		{
			break;
		}
	}

    if (tfp != NULL)
    {
        tfp->close();
        delete tfp;
    }

}
