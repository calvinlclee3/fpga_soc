// Main.c - makes LEDG0 on DE2-115 board blink if NIOS II is set up correctly
// for ECE 385 - University of Illinois - Electrical and Computer Engineering
// Author: Zuofu Cheng
#include <alt_types.h>

int main()
{
	// DEMO PART I
//	int i = 0;
//	volatile unsigned int *LED_PIO = (unsigned int*)0x70; //make a pointer to access the PIO block
//
//	*LED_PIO = 0; //clear all LEDs
//	while ( (1+1) != 3) //infinite loop
//	{
//		for (i = 0; i < 100000; i++); //software delay
//		*LED_PIO |= 0x1; //set LSB
//		for (i = 0; i < 100000; i++); //software delay
//		*LED_PIO &= ~0x1; //clear LSB
//	}
//	return 1; //never gets here

	// DEMO PART II
	alt_u8 acc = 0;
	volatile alt_u32 *SW_PIO = (alt_u32*)0x60;
	volatile alt_u32 *ACCUMULATE_PIO = (alt_u32*)0x50;
	volatile alt_u32 *LED_PIO = (alt_u32*)0x70;

	*LED_PIO = 0; // clear all LEDs

	while ( (1+1) != 3 ) //infinite loop
	{

		if(*ACCUMULATE_PIO == 0x0)
		{
			acc += *SW_PIO;
			*LED_PIO = acc;
			while( (1+1) != 3 )
			{
				if(*ACCUMULATE_PIO == 0x1)
				{
					break;
				}
			}
		}
	}
	return 1; //never gets here
}
