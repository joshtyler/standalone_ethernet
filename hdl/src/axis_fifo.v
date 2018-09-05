module axis_fifo
#(
	parameter AXIS_BYTES = 1,
	parameter DEPTH = 1024
) (
	input clk,
	input sresetn,

	// Input
	output                     axis_i_tready,
	input                      axis_i_tvalid,
	input                      axis_i_tlast,
	input [(AXIS_BYTES*8)-1:0] axis_i_tdata,

	// Output
	input                       axis_o_tready,
	output                      axis_o_tvalid,
	output                      axis_o_tlast,
	output [(AXIS_BYTES*8)-1:0] axis_o_tdata
);

fifo
	#(
		.WIDTH((AXIS_BYTES*8)+1),
		.DEPTH(DEPTH)
	) fifo_inst (
		.clk(clk),
		.n_reset(sresetn),

		.full( !axis_i_tready),
		.wr_en(axis_i_tvalid),
		.data_in ({axis_i_tdata, axis_i_tlast}),

		.rd_en(axis_o_tready),
		.empty(!axis_o_tvalid),
		.data_out ({axis_o_tdata, axis_o_tlast})
	);

endmodule
