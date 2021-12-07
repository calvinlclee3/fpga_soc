module mouse (
	input logic CLK,
	input logic vs,

	// Avalon-MM Slave Signals
	input  logic AVL_READ,					// Avalon-MM Read
	input  logic AVL_WRITE,					// Avalon-MM Write
	input  logic AVL_CS,						// Avalon-MM Chip Select
	input  logic [5:0] AVL_ADDR,			// Avalon-MM Address
	input  logic [7:0] AVL_WRITEDATA,	// Avalon-MM Write Data (Least significant 4 bits)
	output logic [7:0] AVL_READDATA,		// Avalon-MM Read Data (Least significant 4 bits)
	
	output logic [9:0] x_pos_out, y_pos_out
);

/* USB mouse data */
logic [7:0] x_offset, y_offset, button;
logic [15:0] x_offset_s, y_offset_s;

/* absolute cursor coordinates */
logic [15:0] x_pos, y_pos;

initial begin
	x_pos = 16'h00f0;
	y_pos = 16'h00f0;
end

assign x_offset_s = {{8{x_offset[7]}}, x_offset};
assign y_offset_s = {{8{y_offset[7]}}, y_offset};


assign x_pos_out = x_pos[9:0];
assign y_pos_out = y_pos[9:0];

always_ff @ (posedge vs) begin

	
	/* update absolute cursor coordinates */
	
//	x_pos <= x_pos + (x_offset_s >>> 1);
//	y_pos <= y_pos + (y_offset_s >>> 1);

	if(x_offset_s != 16'b1111111111111111 && x_offset_s != 16'b1111111111111110) begin
	
		if(y_offset_s != 16'b1111111111111111 && y_offset_s != 16'b1111111111111110) begin
	
			x_pos <= x_pos + (x_offset_s >>> 2);
			y_pos <= y_pos + (y_offset_s >>> 2);
	
		end	
	
	end	


end

always_ff @ (posedge CLK) begin
	if (AVL_CS && AVL_WRITE) begin	
		case (AVL_ADDR)
			6'b000000: x_offset <= AVL_WRITEDATA;
			6'b000001: y_offset <= AVL_WRITEDATA;
			6'b000010: button <= AVL_WRITEDATA;
			// 6'b000100: x_pos[7:0] <= AVL_WRITEDATA;
			// 6'b000101: x_pos[15:8] <= AVL_WRITEDATA;
			// 6'b000110: y_pos[7:0] <= AVL_WRITEDATA;
			// 6'b000111: y_pos[15:8] <= AVL_WRITEDATA;
			default: ;
		endcase
	end
	if (AVL_CS && AVL_READ) begin
		case (AVL_ADDR)
			6'b000000: AVL_READDATA <= x_offset;
			6'b000001: AVL_READDATA <= y_offset;
			6'b000010: AVL_READDATA <= button;
			6'b000100: AVL_READDATA <= x_pos[7:0];
			6'b000101: AVL_READDATA <= x_pos[9:8]; // Do not change it back to 15, does not work with 15
			6'b000110: AVL_READDATA <= y_pos[7:0];
			6'b000111: AVL_READDATA <= y_pos[9:8]; // Do not change it back to 15, does not work with 15
			default: ;
		endcase
	end
end

endmodule