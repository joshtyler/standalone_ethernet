// Top level ethernet test module

// Output data encapsulated in an ethernet frame

// payload -> framer --> packer_fifo --> spacer --> rmii_to_axis



module eth_test
(
	input clk,

	// RMII interface
	output [1:0] txd,
	output tx_en,
	input [1:0] rxd,
	input crs_dv,
	input rx_er
);

	localparam SRC_MAC = 48'h000102030405;
	localparam DST_MAC = 48'hFFFFFFFFFFFF; //Broadcast
	localparam ETHERTYPE = 16'h88B5; //Local experimental

logic sresetn;

logic       payload_axis_tready;
logic       payload_axis_tvalid;
logic       payload_axis_tlast;
logic [7:0] payload_axis_tdata;

logic       framed_axis_tready;
logic       framed_axis_tvalid;
logic       framed_axis_tlast;
logic [7:0] framed_axis_tdata;

logic       packet_axis_tready;
logic       packet_axis_tvalid;
logic       packet_axis_tlast;
logic [7:0] packet_axis_tdata;

logic       spaced_axis_tready;
logic       spaced_axis_tvalid;
logic       spaced_axis_tlast;
logic [7:0] spaced_axis_tdata;

reset_gen
	#(
		.POLARITY(0), // Active low
		.COUNT(1000)
	) reset_gen_inst (
		.clk(clk),
		.en(1),
		.sreset(sresetn)
	);
/* verilator lint_off WIDTH */
vector_to_axis
	#(
		.VEC_BYTES(128),
		.AXIS_BYTES(1),
		.MSB_FIRST(1)
	) sof_axis_gen (
		.clk(clk),
		.sresetn(sresetn),

		.vec(128'hDEADBEEFCAFECAFEDEADBEEFCAFECAFE),

		.axis_tready(payload_axis_tready),
		.axis_tvalid(payload_axis_tvalid),
		.axis_tlast (payload_axis_tlast),
		.axis_tdata (payload_axis_tdata)
	);
/* verilator lint_on WIDTH */
eth_framer framer (
		.clk(clk),
		.sresetn(sresetn),

		.src_mac(SRC_MAC),
		.dst_mac(DST_MAC),
		.ethertype(ETHERTYPE),

		.payload_axis_tready(payload_axis_tready),
		.payload_axis_tvalid(payload_axis_tvalid),
		.payload_axis_tlast (payload_axis_tlast),
		.payload_axis_tdata (payload_axis_tdata),

		.out_axis_tready(framed_axis_tready),
		.out_axis_tvalid(framed_axis_tvalid),
		.out_axis_tlast (framed_axis_tlast),
		.out_axis_tdata (framed_axis_tdata)
	);

//Replace with packet FIFO!!!!!
axis_fifo
	#(
		.AXIS_BYTES(1)
	) fifo_inst (
		.clk(clk),
		.sresetn(sresetn),

		.axis_i_tready(framed_axis_tready),
		.axis_i_tvalid(framed_axis_tvalid),
		.axis_i_tlast (framed_axis_tlast),
		.axis_i_tdata (framed_axis_tdata),

		.axis_o_tready(packet_axis_tready),
		.axis_o_tvalid(packet_axis_tvalid),
		.axis_o_tlast (packet_axis_tlast),
		.axis_o_tdata (packet_axis_tdata)
	);

	axis_spacer
		#(
			.AXIS_BYTES(1),
			.GAP_CYCLES(5000000)
		) spacer_inst (
			.clk(clk),
			.sresetn(sresetn),

			.axis_i_tready(packet_axis_tready),
			.axis_i_tvalid(packet_axis_tvalid),
			.axis_i_tlast (packet_axis_tlast),
			.axis_i_tdata (packet_axis_tdata),

			.axis_o_tready(spaced_axis_tready),
			.axis_o_tvalid(spaced_axis_tvalid),
			.axis_o_tlast (spaced_axis_tlast),
			.axis_o_tdata (spaced_axis_tdata)
		);

	rmii_to_axis rmii_inst (
			.clk(clk),
			.sresetn(sresetn),

			.txd(txd),
			.tx_en(tx_en),
			.rxd(rxd),
			.crs_dv(crs_dv),
			.rx_er(rx_er),

			.tx_axis_ready (spaced_axis_tready),
			.tx_axis_tvalid(spaced_axis_tvalid),
			.tx_axis_tlast (spaced_axis_tlast),
			.tx_axis_tdata (spaced_axis_tdata),

			.rx_axis_tvalid(),
			.rx_axis_tlast (),
			.rx_axis_tdata ()
		);

endmodule
