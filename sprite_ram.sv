`define SPRITE_H_PIXEL 60
`define SPRITE_V_PIXEL 60
`define SPRITE_COUNT 26

module  sprite_ram
(
	input logic CLK,
	input logic [16:0] ADDR,

	output logic [2:0] data_out
);

/************************************************************************

ADDR[16]    1 iff chess piece is white
ADDR[15]    1 iff chess piece on light background
ADDR[14:12] determines type of chess piece
				0 for null
				1 for pawn
				2 for bishop
				3 for knight
				4 for rook
				5 for queen
				6 for king

ADDR[11:0]  pixel address in raster order from 0 to 3599 inclusive

************************************************************************/
logic [2:0] mem [`SPRITE_H_PIXEL * `SPRITE_V_PIXEL* `SPRITE_COUNT];


initial begin

	 $readmemh("ece385sprites/text/piece_wk_background_l.txt", mem, 0, 3599);
	 $readmemh("ece385sprites/text/piece_wp_background_d.txt", mem, 3600, 7199);
	 
end


always_ff @ (posedge CLK) begin

	data_out <= mem[ADDR];
	
end

endmodule
