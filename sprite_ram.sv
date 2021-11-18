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

ADDR[16]    1 iff chess piece on light background
ADDR[15]    1 iff chess piece is white
ADDR[14:12] determines type of chess piece
				0 for empty
				1 for pawn
				2 for bishop
				3 for knight
				4 for rook
				5 for queen
				6 for king

ADDR[11:0]  pixel address in raster order from 0 to 3599 inclusive

IMPORTANT: for each empty sprites, we load it twice for both white and dark so there is no confusion.

************************************************************************/

logic [2:0] mem [131071];


initial begin
	 
	// Loading empty
	$readmemh("ece385sprites/text/null_background_d.txt", mem, 17'b00000000000000000, 17'b00000111000001111);
	$readmemh("ece385sprites/text/null_background_l.txt", mem, 17'b10000000000000000, 17'b10000111000001111);
	$readmemh("ece385sprites/text/null_background_d.txt", mem, 17'b01000000000000000, 17'b01000111000001111);
	$readmemh("ece385sprites/text/null_background_l.txt", mem, 17'b11000000000000000, 17'b11000111000001111);

	// Loading pawn
	$readmemh("ece385sprites/text/piece_bp_background_d.txt", mem, 17'b00001000000000000, 17'b00001111000001111);
	$readmemh("ece385sprites/text/piece_bp_background_l.txt", mem, 17'b10001000000000000, 17'b10001111000001111);
	$readmemh("ece385sprites/text/piece_wp_background_d.txt", mem, 17'b01001000000000000, 17'b01001111000001111);
	$readmemh("ece385sprites/text/piece_wp_background_l.txt", mem, 17'b11001000000000000, 17'b11001111000001111);

	// Loading bishop
	$readmemh("ece385sprites/text/piece_bb_background_d.txt", mem, 17'b00010000000000000, 17'b00010111000001111);
	$readmemh("ece385sprites/text/piece_bb_background_l.txt", mem, 17'b10010000000000000, 17'b10010111000001111);
	$readmemh("ece385sprites/text/piece_wb_background_d.txt", mem, 17'b01010000000000000, 17'b01010111000001111);
	$readmemh("ece385sprites/text/piece_wb_background_l.txt", mem, 17'b11010000000000000, 17'b11010111000001111);

	// Loading knight
	$readmemh("ece385sprites/text/piece_bn_background_d.txt", mem, 17'b00011000000000000, 17'b00011111000001111);
	$readmemh("ece385sprites/text/piece_bn_background_l.txt", mem, 17'b10011000000000000, 17'b10011111000001111);
	$readmemh("ece385sprites/text/piece_wn_background_d.txt", mem, 17'b01011000000000000, 17'b01011111000001111);
	$readmemh("ece385sprites/text/piece_wn_background_l.txt", mem, 17'b11011000000000000, 17'b11011111000001111);

	// Loading rook
	$readmemh("ece385sprites/text/piece_br_background_d.txt", mem, 17'b00100000000000000, 17'b00100111000001111);
	$readmemh("ece385sprites/text/piece_br_background_l.txt", mem, 17'b10100000000000000, 17'b10100111000001111);
	$readmemh("ece385sprites/text/piece_wr_background_d.txt", mem, 17'b01100000000000000, 17'b01100111000001111);
	$readmemh("ece385sprites/text/piece_wr_background_l.txt", mem, 17'b11100000000000000, 17'b11100111000001111);

	// Loading queen
	$readmemh("ece385sprites/text/piece_bq_background_d.txt", mem, 17'b00101000000000000, 17'b00101111000001111);
	$readmemh("ece385sprites/text/piece_bq_background_l.txt", mem, 17'b10101000000000000, 17'b10101111000001111);
	$readmemh("ece385sprites/text/piece_wq_background_d.txt", mem, 17'b01101000000000000, 17'b01101111000001111);
	$readmemh("ece385sprites/text/piece_wq_background_l.txt", mem, 17'b11101000000000000, 17'b11101111000001111);
	
	// Loading king
	$readmemh("ece385sprites/text/piece_bk_background_d.txt", mem, 17'b00110000000000000, 17'b00110111000001111);
	$readmemh("ece385sprites/text/piece_bk_background_l.txt", mem, 17'b10110000000000000, 17'b10110111000001111);
	$readmemh("ece385sprites/text/piece_wk_background_d.txt", mem, 17'b01110000000000000, 17'b01110111000001111);
	$readmemh("ece385sprites/text/piece_wk_background_l.txt", mem, 17'b11110000000000000, 17'b11110111000001111);
	
end


always_ff @ (posedge CLK) begin

	data_out <= mem[ADDR];
	
end

endmodule
