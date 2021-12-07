module end_game (
	input logic CLK,
    input logic [9:0] DrawX, DrawY,

	// Avalon-MM Slave Signals
	input  logic AVL_WRITE,					// Avalon-MM Write
	input  logic AVL_CS,						// Avalon-MM Chip Select
	input  logic [6:0] AVL_ADDR,			// Avalon-MM Address
	input  logic [7:0] AVL_WRITEDATA,	// Avalon-MM Write Data (Least significant 4 bits)
	
	output logic [1:0] end_game_on,
    output logic [14:0] img_addr
);

logic [1:0] AVL_DATA;

always_comb begin

    if(DrawX >= 10'd120 && DrawX <= 10'd359 && DrawY >= 10'd180 && DrawY <= 10'd299) begin
        
        end_game_on = AVL_DATA;
        if(AVL_DATA == 2'b0)
            img_addr = 15'hx;
        else
            img_addr = (DrawY - 180) * 240 + (DrawX - 120);


    end
        
    else begin
        end_game_on = 2'b00;
        img_addr = 15'hx;
    end
        
end


always_ff @ (posedge CLK) begin

	if (AVL_CS && AVL_WRITE) begin	

		if(AVL_ADDR == 7'b1001000) 
            AVL_DATA <= AVL_WRITEDATA[1:0];

	end

end

endmodule