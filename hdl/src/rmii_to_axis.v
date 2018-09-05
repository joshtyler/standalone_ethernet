// Interface an RMII Phy to two AXIS streams
// Data width is fixed at 8 bits because this is fundamental to how it works
// N.B. This module assumes 100 BASE TX communication

module rmii_to_axis
(
	input refclk, // 50MHz
	input sresetn,

	// RMII interface
	output [1:0] txd,
	output tx_en,
	input [1:0] rxd,
	input crs_dv,
	input rx_er,

	// TX AXIS
	input       tx_axis_tvalid,
	input       tx_axis_tlast, //Not currently used
	input [7:0] tx_axis_tdata,

	// Rx axis
	output       rx_axis_tvalid,
	output       rx_axis_tlast,
	output [7:0] rx_axis_tdata
);

// N.B. Rx interface is not currently implemented
assign rx_axis_tvalid = 0;
assign rx_axis_tlast = 0;
assign rx_axis_tdata = 0;

reg [1 : 0] ctr;

always @(posedge refclk)
begin
	if (sresetn == 0)
	begin
		ctr = 0;
	end else begin
		if (tx_en)
		begin
			ctr <= ctr + 1;
		end
	end
end

assign txd = tx_axis_tdata[ (1+ctr)*2 -: 2];
assign tx_en = tx_axis_tvalid;

endmodule
