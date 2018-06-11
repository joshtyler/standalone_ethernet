VERILATOR = verilator
VERILATOR_FLAGS = --trace --cc --exe

VERILOG_TOP = eth_framer.v
#VERILOG_SOURCES = eth_framer.v vector_to_axis.v axis_joiner.v
CPP_SOURCES = test_eth_framer.cpp

default : run

run:
	$(VERILATOR) $(VERILATOR_FLAGS) $(VERILOG_TOP) $(CPP_SOURCES)

	$(MAKE) -j 4 -C obj_dir -f Veth_framer.mk
