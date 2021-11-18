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




// Local Variables
logic blank, pixel_clk;

logic [9:0] DrawX, DrawY;
logic [3:0] pre_red, pre_green, pre_blue;


logic [11:0] PALETTE_REG [8] = '{ 12'h000, 12'h333, 12'h666, 12'h999, 12'hccc, 12'hfff , 12'h742 , 12'hfa9 };

logic [16:0] ADDR;
logic [2:0] palette_index;


// Submodule Declarations
vga_controller vga_controller0 (.Clk(CLK), .Reset(RESET), .hs(hs), .vs(vs), .pixel_clk(pixel_clk), 
                                .blank(blank), .DrawX(DrawX), .DrawY(DrawY));

sprite_ram ram (.CLK(CLK), .ADDR(ADDR), .data_out(palette_index));

always_comb begin
  
	if(~blank) begin
	
		pre_red = 4'h0;
		pre_green = 4'h0;
		pre_blue = 4'h0;
		ADDR = 17'h0;
	
	end
	else begin

		if(DrawX > 60 || DrawY > 60) begin
		
			pre_red = 4'h0;
			pre_green = 4'h0;
			pre_blue = 4'h0;
			ADDR = 17'h0;
			
		end
		else begin
		
			ADDR = DrawY * 60 + DrawX + 17'b00100000000000000;
			pre_red = PALETTE_REG[palette_index][11:8];
			pre_green = PALETTE_REG[palette_index][7:4];
			pre_blue = PALETTE_REG[palette_index][3:0];
			
		end
		
	end
	
end

always_ff @ (posedge pixel_clk) begin

	red = pre_red;
	green = pre_green;
	blue = pre_blue;

end

endmodule
