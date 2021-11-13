/*
 * ECE385-HelperTools/PNG-To-Txt
 * Author: Rishi Thakkar
 *
 */

module  sprite_ram
(
	input logic [11:0] pixel_address,
	input logic [4:0] sprite_address,
	input logic Clk,

	output logic [2:0] data_Out
);

// mem has width of 3 bits and a total of 3600 addresses
logic [2:0] mem [3600];

initial
begin
	 $readmemh("ece385sprites/text/piece_wk_background_l.txt", mem);
end


always_ff @ (posedge Clk) begin

	data_Out <= mem[pixel_address];
end

endmodule
