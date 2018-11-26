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
	localparam ETHERTYPE = 16'h0800; //Local experimental

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

logic       udp_axis_tready;
logic       udp_axis_tvalid;
logic       udp_axis_tlast;
logic [7:0] udp_axis_tdata;

logic       ip_axis_tready;
logic       ip_axis_tvalid;
logic       ip_axis_tlast;
logic [7:0] ip_axis_tdata;

logic       eth_payload_axis_tready;
logic       eth_payload_axis_tvalid;
logic       eth_payload_axis_tlast;
logic [7:0] eth_payload_axis_tdata;

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

localparam integer PAYLOAD_BYTES = 32;

vector_to_axis
	#(
		.VEC_BYTES(PAYLOAD_BYTES),
		.AXIS_BYTES(1),
		.MSB_FIRST(1)
	) sof_axis_gen (
		.clk(clk),
		.sresetn(sresetn),

		.vec(256'hDEADBEEFCAFECAFEDEADBEEFCAFECAFEDEADBEEFCAFECAFEDEADBEEFCAFECAFE),

		.axis_tready(payload_axis_tready),
		.axis_tvalid(payload_axis_tvalid),
		.axis_tlast (payload_axis_tlast),
		.axis_tdata (payload_axis_tdata)
	);
/* verilator lint_on WIDTH */

udp_header_gen udp_gen (
	.clk(clk),
	.sresetn(sresetn),

	.src_port(100),
	.dest_port(200),

	.payload_length_axis_tready(),
	.payload_length_axis_tvalid(1),
	.payload_length_axis_tlast(1),
	.payload_length_axis_tdata(PAYLOAD_BYTES[15:0]),

	.axis_o_tready(udp_axis_tready),
	.axis_o_tvalid(udp_axis_tvalid),
	.axis_o_tlast (udp_axis_tlast),
	.axis_o_tdata (udp_axis_tdata)
	);

localparam [15:0] IP_BYTES = PAYLOAD_BYTES[15:0]+8;

ip_header_gen ip_gen (
	.clk(clk),
	.sresetn(sresetn),

	.src_ip(32'hC0A80001), //192.168.0.1
	.dest_ip(32'hC0A80002), //192.168.0.2
	.protocol(17),

	.payload_length_axis_tready(),
	.payload_length_axis_tvalid(1),
	.payload_length_axis_tlast(1),
	.payload_length_axis_tdata(IP_BYTES),

	.axis_o_tready(ip_axis_tready),
	.axis_o_tvalid(ip_axis_tvalid),
	.axis_o_tlast (ip_axis_tlast),
	.axis_o_tdata (ip_axis_tdata)
	);

axis_joiner
#(
	.AXIS_BYTES(1),
	.NUM_STREAMS(3)
) input_joiner (
	.clk(clk),
	.sresetn(sresetn),

	.axis_i_tready({ payload_axis_tready,
	                     udp_axis_tready,
	                      ip_axis_tready}),

	 .axis_i_tvalid({ payload_axis_tvalid,
	                      udp_axis_tvalid,
	                       ip_axis_tvalid}),

	.axis_i_tlast({ payload_axis_tlast,
	                    udp_axis_tlast,
	                     ip_axis_tlast}),

	.axis_i_tdata({ payload_axis_tdata,
	                    udp_axis_tdata,
	                     ip_axis_tdata}),

	.axis_o_tready(eth_payload_axis_tready),
	.axis_o_tvalid(eth_payload_axis_tvalid),
	.axis_o_tlast (eth_payload_axis_tlast),
	.axis_o_tdata (eth_payload_axis_tdata)
);

eth_framer framer (
		.clk(clk),
		.sresetn(sresetn),

		.src_mac(SRC_MAC),
		.dst_mac(DST_MAC),
		.ethertype(ETHERTYPE),

		.payload_axis_tready(eth_payload_axis_tready),
		.payload_axis_tvalid(eth_payload_axis_tvalid),
		.payload_axis_tlast (eth_payload_axis_tlast),
		.payload_axis_tdata (eth_payload_axis_tdata),

		.out_axis_tready(framed_axis_tready),
		.out_axis_tvalid(framed_axis_tvalid),
		.out_axis_tlast (framed_axis_tlast),
		.out_axis_tdata (framed_axis_tdata)
	);

axis_packet_fifo
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
