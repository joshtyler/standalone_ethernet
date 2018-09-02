#include <iostream>
#include <iomanip> //setw
#include "Vcrc.h"
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

	Vcrc* top = new Vcrc;
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
	AXISSink<vluint32_t> outAxis(clk, top->sresetn, top->out_axis_tready,
		top->out_axis_tvalid, top->out_axis_tlast, top->out_axis_tdata);

	std::vector<std::vector<vluint8_t>> inData = {{0x0,0x1,0x2,0x3}};
	//std::vector<std::vector<vluint8_t>> inData = {{0x01}};
	AXISSource<vluint8_t> inAxis(clk, top->sresetn, top->in_axis_tready,
		top->in_axis_tvalid, top->in_axis_tlast, top->in_axis_tdata,
		inData);


		top->out_axis_tready = 1;

	while(!Verilated::gotFinish())
	{
		time++;
		top->clk = clk.getVal();

		//Set sresetn
		if(time < 12)
		{
			top->sresetn = 0;
		} else {
			top->sresetn = 1;
		}

		inAxis.eval_in();
		outAxis.eval_in();
		top->eval();
		inAxis.eval_out();
		outAxis.eval_out();


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
			//break;
		}
	}
	// Print first packet
	std::vector<std::vector<vluint32_t>> data = outAxis.getData();
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
