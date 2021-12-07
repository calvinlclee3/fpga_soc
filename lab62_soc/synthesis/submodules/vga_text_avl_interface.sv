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
	input  logic [6:0] AVL_ADDR,			// Avalon-MM Address
	input  logic [7:0] AVL_WRITEDATA,		// Avalon-MM Write Data
	output logic [7:0] AVL_READDATA,		// Avalon-MM Read Data
	
	// Exported Conduit (mapped to VGA port - make sure you export in Platform Designer)
	output logic [3:0]  red, green, blue,	// VGA color channels (mapped to output pins in top-level)
	output logic hs, vs						// VGA HS/VS
);




// Local Variables
logic blank, pixel_clk;
logic [9:0] DrawX, DrawY;
logic [3:0] pre_red, pre_green, pre_blue;

logic [11:0] PALETTE_REG [15] = '{ 12'h000, 12'h111, 12'h222, 12'h333, 12'h444, 12'h555, 12'h666, 12'h777, 12'h999, 12'haaa, 12'hbbb, 12'hccc, 12'hddd, 12'heee, 12'hfff};
logic [11:0] background_colors [4] = '{ 12'h742, 12'hfa9, 12'h8d0, 12'h000 };
logic [1:0] background_index;
logic board_on;

logic [11:0] pixel_addr;
logic [3:0] img_addr;
logic [3:0] palette_index;

logic [9:0] mouseX, mouseY;
logic [3:0] m_palette_index;
logic [7:0] m_pixel_addr;
logic mouse_on;

logic [7:0] board_READDATA, mouse_READDATA;

// Submodule Declarations
vga_controller vga_controller0 (.Clk(CLK), .Reset(RESET), .hs(hs), .vs(vs), .pixel_clk(pixel_clk), 
                                .blank(blank), .DrawX(DrawX), .DrawY(DrawY));

sprite_ram ram (.CLK(CLK), .img_addr(img_addr), .pixel_addr(pixel_addr), .data_out(palette_index));

mouse_sprite_ram mram (.CLK(CLK), .pixel_addr(m_pixel_addr), .data_out(m_palette_index));

board b (.CLK(CLK), .DrawX(DrawX), .DrawY(DrawY), .AVL_ADDR(AVL_ADDR[5:0]),
		   .AVL_READ(AVL_READ), .AVL_WRITE(AVL_WRITE), .AVL_CS(AVL_CS & ~AVL_ADDR[6]), 
		   .AVL_WRITEDATA(AVL_WRITEDATA), .AVL_READDATA(board_READDATA),
		   .pixel_addr(pixel_addr), .img_addr(img_addr), .board_on(board_on), .background_index(background_index));

mouse m (.CLK(CLK), .vs(vs), .AVL_READ(AVL_READ), .AVL_WRITE(AVL_WRITE), 
		 .AVL_CS(AVL_CS & AVL_ADDR[6]), .AVL_ADDR(AVL_ADDR[5:0]),
		 .AVL_WRITEDATA(AVL_WRITEDATA), .AVL_READDATA(mouse_READDATA),
		 .x_pos_out(mouseX), .y_pos_out(mouseY)); 

always_comb begin
	
	/* MUX to select AVL_READDATA */
	if(AVL_ADDR[6])
		AVL_READDATA = mouse_READDATA;
	else
		AVL_READDATA = board_READDATA;

	/* Determine whether the cursor should be drawn */
	if ((DrawX - mouseX) >= 0 && (DrawX - mouseX) < 15 && 
		(DrawY - mouseY) >= 0 && (DrawY - mouseY) < 15)
	begin
		m_pixel_addr = (DrawY - mouseY) * 15 + (DrawX - mouseX);
		if(m_palette_index == 4'hf)
			mouse_on = 1'b0;
		else
			mouse_on = 1'b1;
	end
	else
	begin
		mouse_on = 1'b0;
		m_pixel_addr = 8'hx;
	end

end

always_comb begin
  
	if(~blank) begin
	
		pre_red = 4'h0;
		pre_green = 4'h0;
		pre_blue = 4'h0;
	
	end
	else begin

		if(mouse_on)
		begin
			pre_red = PALETTE_REG[m_palette_index][11:8];
			pre_green = PALETTE_REG[m_palette_index][7:4];
			pre_blue = PALETTE_REG[m_palette_index][3:0];	
		end
		else if(board_on)
		begin
			if (palette_index == 4'hf)
			begin
				pre_red = background_colors[background_index][11:8];
				pre_green = background_colors[background_index][7:4];
				pre_blue = background_colors[background_index][3:0];
			end
			else
			begin
				pre_red = PALETTE_REG[palette_index][11:8];
				pre_green = PALETTE_REG[palette_index][7:4];
				pre_blue = PALETTE_REG[palette_index][3:0];
			end
		end
		else
		begin
			pre_red = 4'h0;
			pre_green = 4'h0;
			pre_blue = 4'h0;
		end
		
	end
	
end

always_ff @ (posedge pixel_clk) begin

	red <= pre_red;
	green <= pre_green;
	blue <= pre_blue;

end

endmodule
