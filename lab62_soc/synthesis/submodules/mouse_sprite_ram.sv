module  mouse_sprite_ram
(
	input logic CLK,
	input logic [7:0] pixel_addr,

	output logic [3:0] data_out
);

logic [3:0] mem [225];

initial begin
    $readmemh("ece385sprites/text/cursor.txt", mem);
end

always_ff @ (posedge CLK) begin

	data_out <= mem[pixel_addr];
	
end

endmodule