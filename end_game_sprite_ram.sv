module end_game_sprite_ram
(
	input logic CLK,
    input logic [1:0] select,
	input logic [14:0] pixel_addr,

	output logic [3:0] data_out
);

logic [3:0] black [28800];
logic [3:0] white [28800];

initial begin
    $readmemh("ece385sprites/text/end_b.txt", black);
    $readmemh("ece385sprites/text/end_w.txt", white);
end

always_ff @ (posedge CLK) begin

    if(select == 2'b01)
        data_out <= black[pixel_addr];
    else if(select == 2'b10)
        data_out <= white[pixel_addr];
    else 
        data_out <= 15'hx;
	
end

endmodule