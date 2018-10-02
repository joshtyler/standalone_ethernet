VERILATOR = verilator
#VERILATOR_FLAGS = --trace --cc --exe -CFLAGS '-Wall -Werror -Wextra -g'
VERILATOR_FLAGS = --trace --cc --exe --l2-name v -CFLAGS '-Wall -Wextra -g' -Ihdl_common/synth/other -Ihdl_common/synth/axis -Ihdl_common/synth/network

VERILOG_TOP = eth_test
VERILOG_SOURCES = eth_test.sv
#VERILOG_SOURCES = eth_framer.v axis_distributor.v axis_joiner.v axis_unpacker.v crc.v vector_to_axis.v
CPP_SOURCES = test_eth_test.cpp

default : run

run:
	$(VERILATOR) $(VERILATOR_FLAGS) $(VERILOG_SOURCES) --top-module $(VERILOG_TOP) $(CPP_SOURCES)

	$(MAKE) -j 4 -C obj_dir -f Veth_test.mk

clean:
	rm -r obj_dir
