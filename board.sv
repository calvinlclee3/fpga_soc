module board (
	input logic CLK,

	input logic [9:0] DrawX, DrawY,
	
	// Avalon-MM Slave Signals
	input  logic AVL_READ,					// Avalon-MM Read
	input  logic AVL_WRITE,					// Avalon-MM Write
	input  logic AVL_CS,						// Avalon-MM Chip Select
	input  logic [5:0] AVL_ADDR,			// Avalon-MM Address
	input  logic [7:0] AVL_WRITEDATA,	// Avalon-MM Write Data (Least significant 4 bits)
	output logic [7:0] AVL_READDATA,		// Avalon-MM Read Data (Least significant 4 bits)
	
	output logic [11:0] pixel_addr,
	output logic [3:0] img_addr,
	output logic board_on,
	output logic [1:0] background_index
);

logic [3:0] BOARD [64];
logic [2:0] BoardX;
logic [2:0] BoardY;
logic highlight [64];

always_comb
begin

	BoardX = DrawX / 60;
	BoardY = DrawY / 60;
	if (highlight[BoardY * 8 + BoardX]) 
	begin
		background_index = 2'b10;
	end
	else
	begin
		background_index[1] = 1'b0;
		background_index[0] = BoardX[0] ^ BoardY[0];
	end

	if (DrawX >= 480) begin
		pixel_addr = 12'hx;
		img_addr = 4'hx;
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
		BOARD[AVL_ADDR] <= AVL_WRITEDATA[3:0];
		highlight[AVL_ADDR] <= AVL_WRITEDATA[4];
	end
	if (AVL_CS && AVL_READ) begin
		AVL_READDATA[3:0] <= BOARD[AVL_ADDR];
		AVL_READDATA[4] <= highlight[AVL_ADDR];
	end
end

endmodule