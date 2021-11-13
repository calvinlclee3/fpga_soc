/************************************************************************
Avalon-MM Interface VGA Text mode display

Register Map:
0x000-0x0257 : VRAM, 80x30 (2400 byte, 600 word) raster order (first column then row)
0x258        : control register

VRAM Format:
X->
[ 31  30-24][ 23  22-16][ 15  14-8 ][ 7    6-0 ]
[IV3][CODE3][IV2][CODE2][IV1][CODE1][IV0][CODE0]

IVn = Draw inverse glyph
CODEn = Glyph code from IBM codepage 437

Control Register Format:
[[31-25][24-21][20-17][16-13][ 12-9][ 8-5 ][ 4-1 ][   0    ] 
[[RSVD ][FGD_R][FGD_G][FGD_B][BKG_R][BKG_G][BKG_B][RESERVED]

VSYNC signal = bit which flips on every Vsync (time for new frame), used to synchronize software
BKG_R/G/B = Background color, flipped with foreground when IVn bit is set
FGD_R/G/B = Foreground color, flipped with background when Inv bit is set

************************************************************************/
`define NUM_REGS 601 //80*30 characters / 4 characters per register
`define CTRL_REG 600 //index of control register

module vga_text_avl_interface (
	// Avalon Clock Input, note this clock is also used for VGA, so this must be 50Mhz
	// We can put a clock divider here in the future to make this IP more generalizable
	input logic CLK,
	
	// Avalon Reset Input
	input logic RESET,
	
	// Avalon-MM Slave Signals
	input  logic AVL_READ,					// Avalon-MM Read
	input  logic AVL_WRITE,					// Avalon-MM Write
	input  logic AVL_CS,					// Avalon-MM Chip Select
	input  logic [3:0] AVL_BYTE_EN,			// Avalon-MM Byte Enable
	input  logic [11:0] AVL_ADDR,			// Avalon-MM Address
	input  logic [31:0] AVL_WRITEDATA,		// Avalon-MM Write Data
	output logic [31:0] AVL_READDATA,		// Avalon-MM Read Data
	
	// Exported Conduit (mapped to VGA port - make sure you export in Platform Designer)
	output logic [3:0]  red, green, blue,	// VGA color channels (mapped to output pins in top-level)
	output logic hs, vs						// VGA HS/VS
);



//		logic [31:0] LOCAL_REG [`NUM_REGS]; 

// Local Variables
logic blank;
logic [9:0] DrawX, DrawY;
logic [10:0] sprite_addr;
logic [7:0] sprite_data;

logic [9:0] Row, Col;
logic [10:0] VRAM_reg_index;  // Wk2
logic [6:0] glypth_index;
logic bit_to_draw;

logic [31:0] VGA_DATA;
logic [3:0] pre_red, pre_green, pre_blue;
logic pixel_clk;


logic [31:0] PALETTE_REG [8]; // Wk2
logic [3:0] FGD_IDX, BKG_IDX; // Wk2


// On-Chip Memory
// Port A for Avalon
// Port B for VGA  
ram ram_0 (.address_a(AVL_ADDR[10:0]), .data_a(AVL_WRITEDATA), .byteena_a(AVL_BYTE_EN), .wren_a(AVL_WRITE & AVL_CS & (~AVL_ADDR[11])), .q_a(AVL_READDATA),
			  .address_b(VRAM_reg_index), .data_b(), .byteena_b(), .wren_b(), .q_b(VGA_DATA), // Wk2
			  .clock(CLK)); 
			  
			  
//		module ram (
//			address_a,
//			address_b,
//			byteena_a,
//			byteena_b,
//			clock,
//			data_a,
//			data_b,
//			wren_a,
//			wren_b,
//			q_a,
//			q_b);



// Submodule Declarations
vga_controller vga_controller0 (.Clk(CLK), .Reset(RESET), .hs(hs), .vs(vs), .pixel_clk(pixel_clk), 
                                .blank(blank), .DrawX(DrawX), .DrawY(DrawY));

font_rom font_rom0 (.addr(sprite_addr), .data(sprite_data));
//		module  vga_controller ( input        Clk,       // 50 MHz clock
//														  Reset,     // reset signal
//										 output logic hs,        // Horizontal sync pulse.  Active low
//														  vs,        // Vertical sync pulse.  Active low
//														  pixel_clk, // 25 MHz pixel clock output
//														  blank,     // Blanking interval indicator.  Active low.
//														  sync,      // Composite Sync signal.  Active low.  We don't use it in this lab,
//																		 //   but the video DAC on the DE2 board requires an input for it.
//										 output [9:0] DrawX,     // horizontal coordinate
//														  DrawY );   // vertical coordinate
//
//
//		module font_rom ( input [10:0]	addr,
//								output [7:0]	data
//							 );
   
// Palette Register Write Logic (Wk2)

always_ff @(posedge CLK) begin
	

	if(AVL_CS == 1'b1 && AVL_WRITE == 1'b1 && AVL_ADDR[11] == 1'b1) begin
	
		if(AVL_BYTE_EN[0])
			PALETTE_REG[AVL_ADDR[2:0]][7:0] <= AVL_WRITEDATA[7:0];
		if(AVL_BYTE_EN[1])
			PALETTE_REG[AVL_ADDR[2:0]][15:8] <= AVL_WRITEDATA[15:8];
		if(AVL_BYTE_EN[2])
			PALETTE_REG[AVL_ADDR[2:0]][23:16] <= AVL_WRITEDATA[23:16];
		if(AVL_BYTE_EN[3])
			PALETTE_REG[AVL_ADDR[2:0]][31:24] <= AVL_WRITEDATA[31:24];
		
	end

end	
	
// Text Mode Logic

always_comb begin

	Row = DrawY >> 4;
	Col = DrawX >> 3;
	VRAM_reg_index = (Row * 40) + (Col >> 1); // Changed from 20 and 2 (Wk2)
	
	if((Col & 10'b0000000001) == 10'h000) begin
		glypth_index = VGA_DATA[14:8];
		FGD_IDX = VGA_DATA[7:4];
		BKG_IDX = VGA_DATA[3:0];
	end
		
	else begin
		glypth_index = VGA_DATA[30:24];
		FGD_IDX = VGA_DATA[23:20];
		BKG_IDX = VGA_DATA[19:16];		
	end

	
	sprite_addr = (DrawY - (Row << 4)) + (glypth_index << 4);
	
	bit_to_draw = sprite_data[8'h7 - (DrawX - (Col << 3))];

	
end

always_comb begin

	if(~blank) begin
	
		pre_red = 4'h0;
		pre_green = 4'h0;
		pre_blue = 4'h0;
	
	end
	else begin
		
		if(bit_to_draw) begin
		
			if((FGD_IDX & 4'b0001) == 4'b0000) begin //EVEN
			
				pre_red = PALETTE_REG[FGD_IDX >> 1][12:9];
				pre_green = PALETTE_REG[FGD_IDX >> 1][8:5];
				pre_blue = PALETTE_REG[FGD_IDX >> 1][4:1];
				
			end
			else begin  //ODD
			
				pre_red = PALETTE_REG[FGD_IDX >> 1][24:21];
				pre_green = PALETTE_REG[FGD_IDX >> 1][20:17];
				pre_blue = PALETTE_REG[FGD_IDX >> 1][16:13];
				
			end

			
		end
		else begin

			if((BKG_IDX & 4'b0001) == 4'b0000) begin
			
				pre_red = PALETTE_REG[BKG_IDX >> 1][12:9];
				pre_green = PALETTE_REG[BKG_IDX >> 1][8:5];
				pre_blue = PALETTE_REG[BKG_IDX >> 1][4:1];
				
			end
			else begin
			
				pre_red = PALETTE_REG[BKG_IDX >> 1][24:21];
				pre_green = PALETTE_REG[BKG_IDX >> 1][20:17];
				pre_blue = PALETTE_REG[BKG_IDX >> 1][16:13];
				
			end
			
		end
		
	end

end

// DEBUG ONLY: Actual Palette Write is now done via Software

//assign PALETTE_REG[0][12:9] = 4'h0;
//assign PALETTE_REG[0][8:5] = 4'h0;
//assign PALETTE_REG[0][4:1] = 4'h0;
//assign PALETTE_REG[0][24:21] = 4'h0;
//assign PALETTE_REG[0][20:17] = 4'h0;
//assign PALETTE_REG[0][16:13] = 4'ha;
//assign PALETTE_REG[1][12:9] = 4'h0;
//assign PALETTE_REG[1][8:5] = 4'ha;
//assign PALETTE_REG[1][4:1] = 4'h0;
//assign PALETTE_REG[1][24:21] = 4'h0;
//assign PALETTE_REG[1][20:17] = 4'ha;
//assign PALETTE_REG[1][16:13] = 4'ha;
//assign PALETTE_REG[2][12:9] = 4'ha;
//assign PALETTE_REG[2][8:5] = 4'h0;
//assign PALETTE_REG[2][4:1] = 4'h0;
//assign PALETTE_REG[2][24:21] = 4'ha;
//assign PALETTE_REG[2][20:17] = 4'h0;
//assign PALETTE_REG[2][16:13] = 4'ha;
//assign PALETTE_REG[3][12:9] = 4'ha;
//assign PALETTE_REG[3][8:5] = 4'h5;
//assign PALETTE_REG[3][4:1] = 4'h0;
//assign PALETTE_REG[3][24:21] = 4'ha;
//assign PALETTE_REG[3][20:17] = 4'ha;
//assign PALETTE_REG[3][16:13] = 4'ha;
//assign PALETTE_REG[4][12:9] = 4'h5;
//assign PALETTE_REG[4][8:5] = 4'h5;
//assign PALETTE_REG[4][4:1] = 4'h5;
//assign PALETTE_REG[4][24:21] = 4'h5;
//assign PALETTE_REG[4][20:17] = 4'h5;
//assign PALETTE_REG[4][16:13] = 4'hf;
//assign PALETTE_REG[5][12:9] = 4'h5;
//assign PALETTE_REG[5][8:5] = 4'hf;
//assign PALETTE_REG[5][4:1] = 4'h5;
//assign PALETTE_REG[5][24:21] = 4'h5;
//assign PALETTE_REG[5][20:17] = 4'hf;
//assign PALETTE_REG[5][16:13] = 4'hf;
//assign PALETTE_REG[6][12:9] = 4'hf;
//assign PALETTE_REG[6][8:5] = 4'h5;
//assign PALETTE_REG[6][4:1] = 4'h5;
//assign PALETTE_REG[6][24:21] = 4'hf;
//assign PALETTE_REG[6][20:17] = 4'h5;
//assign PALETTE_REG[6][16:13] = 4'hf;
//assign PALETTE_REG[7][12:9] = 4'hf;
//assign PALETTE_REG[7][8:5] = 4'hf;
//assign PALETTE_REG[7][4:1] = 4'h5;
//assign PALETTE_REG[7][24:21] = 4'hf;
//assign PALETTE_REG[7][20:17] = 4'hf;
//assign PALETTE_REG[7][16:13] = 4'hf;


always_ff @ (posedge pixel_clk) begin

	red = pre_red;
	green = pre_green;
	blue = pre_blue;

end

endmodule
