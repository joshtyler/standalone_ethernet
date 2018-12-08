# This file is adapted from project icestorm example
# It is licensed under the ISC license:

#  Permission to use, copy, modify, and/or distribute this software for any
#  purpose with or without fee is hereby granted, provided that the above
#  copyright notice and this permission notice appear in all copies.
#
#  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
#  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
#  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
#  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
#  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
#  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
#  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.


# Constants
SRC_DIR = ../src
PROJ = eth_test
DEVICE = 8k
PACKAGE = ct256

COMMON_SYNTH_DIR = $(SRC_DIR)/hdl_common/synth

# Input files
SRC_FILES = $(SRC_DIR)/eth_test.sv
SRC_FILES += $(COMMON_SYNTH_DIR)/axis/axis_packet_fifo.sv
SRC_FILES += $(COMMON_SYNTH_DIR)/axis/axis_joiner.sv
SRC_FILES += $(COMMON_SYNTH_DIR)/axis/axis_register.sv
SRC_FILES += $(COMMON_SYNTH_DIR)/axis/axis_spacer.sv
SRC_FILES += $(COMMON_SYNTH_DIR)/axis/axis_width_converter.sv
SRC_FILES += $(COMMON_SYNTH_DIR)/axis/vector_to_axis.sv
SRC_FILES += $(COMMON_SYNTH_DIR)/axis/axis_counter.sv
SRC_FILES += $(COMMON_SYNTH_DIR)/axis/axis_round_robin.sv
SRC_FILES += $(COMMON_SYNTH_DIR)/axis/axis_fifo.sv
SRC_FILES += $(COMMON_SYNTH_DIR)/axis/axis_broadcaster.sv
SRC_FILES += $(COMMON_SYNTH_DIR)/axis/axis_padder.sv
SRC_FILES += $(COMMON_SYNTH_DIR)/axis/axis_gater.sv
SRC_FILES += $(COMMON_SYNTH_DIR)/axis/axis_switch.sv

SRC_FILES += $(COMMON_SYNTH_DIR)/network/eth_crc.sv
SRC_FILES += $(COMMON_SYNTH_DIR)/network/eth_framer.sv
SRC_FILES += $(COMMON_SYNTH_DIR)/network/rmii_to_axis.sv
SRC_FILES += $(COMMON_SYNTH_DIR)/network/ip_header_gen.sv
SRC_FILES += $(COMMON_SYNTH_DIR)/network/udp_checksum.sv
SRC_FILES += $(COMMON_SYNTH_DIR)/network/udp_header_gen.sv

SRC_FILES += $(COMMON_SYNTH_DIR)/other/fifo.sv
SRC_FILES += $(COMMON_SYNTH_DIR)/other/reset_gen.sv




PCF_FILE = $(SRC_DIR)/pins.pcf

.PHONY: all
all: $(PROJ).rpt $(PROJ).bin

%.blif: $(SRC_FILES)
	@echo "Begin synthesis"
	yosys -p "synth_ice40 -blif $(PROJ).blif" $(SRC_FILES) > yosys.log
	@echo "Yosys warnings:"
	@cat yosys.log | grep -in "warning"

%.asc: $(PCF_FILE) %.blif
	@echo "Begin pnr"
	arachne-pnr -d $(DEVICE) -p $(PACKAGE) -o $(PROJ).asc -p $(PCF_FILE) $(PROJ).blif

%.bin: %.asc
	icepack $< $@

%.rpt: %.asc
	icetime -d hx$(DEVICE) -mtr $@ $<

prog: $(PROJ).bin
	iceprog $<

clean:
	rm -f $(PROJ).blif $(PROJ).asc $(PROJ).rpt $(PROJ).bin

.SECONDARY:
.PHONY: all clean prog time
