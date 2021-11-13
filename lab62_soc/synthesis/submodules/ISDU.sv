//------------------------------------------------------------------------------
// Company:          UIUC ECE Dept.
// Engineer:         Stephen Kempf
//
// Create Date:    17:44:03 10/08/06
// Design Name:    ECE 385 Lab 6 Given Code - Incomplete ISDU
// Module Name:    ISDU - Behavioral
//
// Comments:
//    Revised 03-22-2007
//    Spring 2007 Distribution
//    Revised 07-26-2013
//    Spring 2015 Distribution
//    Revised 02-13-2017
//    Spring 2017 Distribution
//------------------------------------------------------------------------------


module ISDU (   input logic         Clk, 
									Reset,
					 output logic load_ADDR, Ready
				);

	enum logic [3:0] {  LOAD, 
						WAIT0, 
						WAIT1,
						WAIT2,
						READ}   State, Next_state;   // Internal state logic
		
	always_ff @ (posedge Clk)
	begin
		if (Reset) 
			State <= LOAD;
		else 
			State <= Next_state;
	end
   
	always_comb
	begin 
		// Default next state is staying at current state
		Next_state = State;
		
		load_ADDR = 1'b0;
		Ready = 1'b0;
	
		// Assign next state
		unique case (State)
			LOAD : 

				Next_state = WAIT0;                      
			WAIT0 : 
				Next_state = WAIT1;
			WAIT1:
				Next_state = WAIT2;
			WAIT2:
				Next_state = READ;
			// Any states involving SRAM require more than one clock cycles.
			// The exact number will be discussed in lecture.
			READ : 
				Next_state = LOAD;
			

			default : ;

		endcase
		
		// Assign control signals based on current state
		case (State)
			LOAD: 
				load_ADDR = 1'b1;
			READ : 
				Ready = 1'b1;
			

			default : ;
		endcase
	end 

	
endmodule
