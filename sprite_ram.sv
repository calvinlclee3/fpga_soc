`define SPRITE_H_PIXEL 60
`define SPRITE_V_PIXEL 60
`define SPRITE_COUNT 26

module  sprite_ram
(
	input logic CLK,
	input logic [3:0] img_addr,
	input logic [11:0] pixel_addr,

	output logic [3:0] data_out
);

/************************************************************************

img_addr[0]    1 iff chess piece is white
img_addr[3:1] 	determines type of chess piece
				0 for pawn
				1 for bishop
				2 for knight
				3 for rook
				4 for queen
				5 for king

pass into img_addr the value 12 to blank the square with just the background

pixel_addr[11:0]	pixel address in raster order from 0 to 3599 inclusive

IMPORTANT: changed the system so that background is now transparent.

************************************************************************/

logic [3:0] mem [46800];

initial begin

	// Loading pawn
	$readmemh("ece385sprites/text/piece_bp.txt", mem, 0, 3599);
	$readmemh("ece385sprites/text/piece_wp.txt", mem, 3600, 7199);

	// Loading bishop
	$readmemh("ece385sprites/text/piece_bb.txt", mem, 7200, 10799);
	$readmemh("ece385sprites/text/piece_wb.txt", mem, 10800, 14399);

	// Loading knight
	$readmemh("ece385sprites/text/piece_bn.txt", mem, 14400, 17999);
	$readmemh("ece385sprites/text/piece_wn.txt", mem, 18000, 21599);

	// Loading rook
	$readmemh("ece385sprites/text/piece_br.txt", mem, 21600, 25199);
	$readmemh("ece385sprites/text/piece_wr.txt", mem, 25200, 28799);

	// Loading queen
	$readmemh("ece385sprites/text/piece_bq.txt", mem, 28800, 32399);
	$readmemh("ece385sprites/text/piece_wq.txt", mem, 32400, 35999);
	
	// Loading king
	$readmemh("ece385sprites/text/piece_bk.txt", mem, 36000, 39599);
	$readmemh("ece385sprites/text/piece_wk.txt", mem, 39600, 43199);
	
	// Load blank square
	$readmemh("ece385sprites/text/blank.txt", mem, 43200, 46799);
	
end

always_ff @ (posedge CLK) begin

	data_out <= mem[img_addr * 3600 + pixel_addr];
	
end

endmodule
