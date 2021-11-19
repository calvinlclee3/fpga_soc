module board (
	input logic CLK,

	input logic [9:0] DrawX, DrawY,
	
	// Avalon-MM Slave Signals
	input  logic AVL_READ,					// Avalon-MM Read
	input  logic AVL_WRITE,					// Avalon-MM Write
	input  logic AVL_CS,						// Avalon-MM Chip Select
	input  logic [5:0] AVL_ADDR,			// Avalon-MM Address
	input  logic [3:0] AVL_WRITEDATA,	// Avalon-MM Write Data (Least significant 4 bits)
	output logic [3:0] AVL_READDATA,		// Avalon-MM Read Data (Least significant 4 bits)
	
	output logic [11:0] pixel_addr,
	output logic [3:0] img_addr,
	output logic board_on,
	output logic background_index
);

logic [3:0] BOARD [64];
logic [2:0] BoardX;
logic [2:0] BoardY;

initial begin
	BOARD = '{ 4'h6, 4'h4, 4'h2, 4'h8, 4'ha, 4'h2, 4'h4, 4'h6,
				4'h0, 4'h0, 4'h0, 4'h0, 4'h0, 4'h0, 4'h0, 4'h0, 
				4'hc, 4'hc, 4'hc, 4'hc, 4'hc, 4'hc, 4'hc, 4'hc, 
				4'hc, 4'hc, 4'hc, 4'hc, 4'hc, 4'hc, 4'hc, 4'hc, 
				4'hc, 4'hc, 4'hc, 4'hc, 4'hc, 4'hc, 4'hc, 4'hc,
				4'hc, 4'hc, 4'hc, 4'hc, 4'hc, 4'hc, 4'hc, 4'hc,
				4'h1, 4'h1, 4'h1, 4'h1, 4'h1, 4'h1, 4'h1, 4'h1,
				4'h7, 4'h5, 4'h3, 4'h9, 4'hb, 4'h3, 4'h5, 4'h7 };
end

always_comb
begin

	BoardX = DrawX / 60;
	BoardY = DrawY / 60;
	background_index = BoardX[0] ^ BoardY[0];

	if (DrawX >= 480) begin
		pixel_addr = 12'hx;
		img_addr = 12'hx;
		board_on = 1'b0;
	end
	else begin
		pixel_addr = (DrawY % 60) * 60 + (DrawX % 60);
		img_addr = BOARD[BoardY * 8 + BoardX];
		board_on = 1'b1;
	end
end

always_ff @ (posedge CLK) begin
	if (AVL_CS && AVL_WRITE) begin
		BOARD[AVL_ADDR] <= AVL_WRITEDATA;
	end
	if (AVL_CS && AVL_READ) begin
		AVL_READDATA <= BOARD[AVL_ADDR];
	end
end

endmodule